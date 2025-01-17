
#include <toollib/ToolCommand.hpp>

#include "argparse/argparse.hpp"

struct TestCmd : tool::IToolCommand {
	TestCmd()
		: parser("test", "", argparse::default_arguments::help) {
		// clang-format off
		parser
			.add_description("Does something cool");


		// FIXME: Probably just print this always, in a thinner format, but use
		// the existing thicker format for verbosity.
		parser
			.add_argument("--verbose")
			.help("Increase information output verbosity")
			.default_value(false)
			.implicit_value(true);
		// clang-format on
	}

	void Init(argparse::ArgumentParser& parent) override {
		parent.add_subparser(parser);
	}

	bool ShouldRun(argparse::ArgumentParser& parent) const override {
		return parent.is_subcommand_used("test");
	}

	int Parse() override {
		verbose = parser.get<bool>("--verbose");
		return 0;
	}

	int Run() override {
		std::printf("Pretend this does something useful\n");

		if(verbose)
			std::printf("I printed you a cake! it might be a lie thogh x3\n");
		return 0;
	}

   private:
	argparse::ArgumentParser parser;

	// Parsed arguments
	bool verbose = false;
};

TOOLLIB_REGISTER_TOOLCOMMAND("test", TestCmd);