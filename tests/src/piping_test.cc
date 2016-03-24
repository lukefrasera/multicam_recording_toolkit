#include <pstreams/pstream.h>


int main(int argc, char *argv[]) {
  redi::opstream pout("grep -inI h");

  pout << "hello world\n I am !";
  return 0;
}