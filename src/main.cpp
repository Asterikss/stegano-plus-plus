#include "execute.hpp"

int main(int argc, char *arqv[]) {
  if (!execute::ex_parse(argc, arqv)) {
    return 1;
  }
  execute::execute();
}
