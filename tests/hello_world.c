#include <stdio.h>
#include <link.h>


void print_hello_world(void)
{
    puts("Hello world");
}

int main(void)
{
    printf("%p\n", (Elf64_Phdr*)_DYNAMIC);

    print_hello_world();
}
