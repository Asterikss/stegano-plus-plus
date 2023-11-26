#include "headers/execute.hpp"
// TODO file size changes for some reason for ppms

int main(int argc, char *arqv[]){
	if (!execute::ex_parse(argc, arqv))		{return 1;}
	execute::execute();
}

