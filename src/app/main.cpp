#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>

#include <chain.h>

#include "utils.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace json = boost::json;
namespace po = boost::program_options;
namespace pt = boost::property_tree;

using tcp = boost::asio::ip::tcp;

namespace node_state
{
	auto chain = gnome::chain();
}

class http_connection : public std::enable_shared_from_this<http_connection>
{
public:
	http_connection(tcp::socket socket)
		: socket_(std::move(socket))
	{}

	// Initiate the asynchronous operations associated with the connection.
	void start()
	{
		read_request();
		check_deadline();
	}

private:
	// The socket for the currently connected client.
	tcp::socket socket_;

	// The buffer for performing reads.
	beast::flat_buffer buffer_{ 8192 };

	// The request message.
	http::request<http::string_body> request_;

	// The response message.
	http::response<http::string_body> response_;

	// The timer for putting a deadline on connection processing.
	net::steady_timer deadline_{
		socket_.get_executor(), std::chrono::seconds(60) };

	// Asynchronously receive a complete request message.
	void read_request()
	{
		auto self = shared_from_this();

		http::async_read(
			socket_,
			buffer_,
			request_,
			[self] (beast::error_code ec, std::size_t bytes_transferred)
			{
				boost::ignore_unused(bytes_transferred);
				if (!ec)
					self->process_request();
			});
	}

	// Determine what needs to be done with the request message.
	void process_request()
	{
		response_.version(request_.version());
		response_.keep_alive(false);

		switch (request_.method())
		{
			case http::verb::get:
			response_.result(http::status::ok);
			response_.set(http::field::server, "gnomecrypto");
			create_response();
			break;

			default:
			json::value j = { "error", "invalid request" };
			response_.result(http::status::not_found);
			response_.set(http::field::content_type, "application/json");
			response_.body() = serialize(j);
			break;
		}

		write_response();
	}

	// Construct a response message based on the program state.
	void create_response()
	{
		const auto api = request_.target();
		try
		{
			auto body = read_json(request_.body());
			
			if (api == "/chain")
			{
				const auto j = node_state::chain.to_json();
				response_.set(http::field::content_type, "application/json");
				response_.body() = serialize(j);
				return;
			}
			if (api == "/mine")
			{
				if (auto d = body.get_child_optional("payload"))
				{
					const auto blk = node_state::chain.append(d->data());
					const auto ret = blk.to_json();
					response_.set(http::field::content_type, "application/json");
					response_.body() = serialize(ret);
					return;
				}
			}
		}
		catch (pt::json_parser_error e)
		{
		}
		json::value j = { "error", "invalid request" };
		response_.result(http::status::not_found);
		response_.set(http::field::content_type, "application/json");
		response_.body() = serialize(j);
	}

	// Asynchronously transmit the response message.
	void write_response()
	{
		auto self = shared_from_this();

		response_.content_length(response_.body().size());

		http::async_write(
			socket_,
			response_,
			[self] (beast::error_code ec, std::size_t)
			{
				self->socket_.shutdown(tcp::socket::shutdown_send, ec);
				self->deadline_.cancel();
			});
	}

	// Check whether we have spent enough time on this connection.
	void check_deadline()
	{
		auto self = shared_from_this();

		deadline_.async_wait(
			[self] (beast::error_code ec)
			{
				if (!ec)
				{
					// Close socket to cancel any outstanding operation.
					self->socket_.close(ec);
				}
			});
	}
};

// "Loop" forever accepting new connections.
void http_server(tcp::acceptor &acceptor, tcp::socket &socket)
{
	acceptor.async_accept(socket,
		[&] (beast::error_code ec)
		{
			if (!ec)
				std::make_shared<http_connection>(std::move(socket))->start();
			http_server(acceptor, socket);
		});
}

int main(int argc, char *argv[])
{
	try
	{
		po::options_description desc("optional arguments");
		desc.add_options()
			("help,h", "show help message and exit.")
			("address,a", po::value<std::string>()->default_value("0.0.0.0"), "local network address for this node")
			("port,p", po::value<unsigned short>()->default_value(7420), "port for this node")
			;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help"))
		{
			std::cout << desc << "\n";
			return EXIT_SUCCESS;
		}

		auto const address = net::ip::make_address(vm["address"].as<std::string>());
		auto port = vm["port"].as<unsigned short>();

		net::io_context ioc;

		tcp::acceptor acceptor{ ioc, {address, port} };
		tcp::socket socket{ ioc };
		http_server(acceptor, socket);

		ioc.run();
	}
	catch (std::exception const &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}