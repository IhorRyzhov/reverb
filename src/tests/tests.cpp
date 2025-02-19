#include "minunit/minunit.h"
#include "../liquid-dsp/include/liquid.h"
#include "../reverb/upolsconv.hpp"

MU_TEST(test_check) {
	UpolsConv(16);
}
MU_TEST_SUITE(test_suite) {
	MU_RUN_TEST(test_check);
}

int main(int argc, char *argv[]) {

	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}