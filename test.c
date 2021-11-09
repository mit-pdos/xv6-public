#include "types.h"
#include "user.h"

int main() {

  void* region = getSharedPage(0, 1);


	for (int i = 0; i < 10; i++) {
		printf(1, "%d ", ((char*)region)[i]);
	}
	printf(1, "\n");

  // write
  strcpy(region, "region");

  // read
  printf(1, "%s\n", region);

	// FreeSharedPage(0);

  exit();
}
