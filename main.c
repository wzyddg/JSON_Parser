#include <stdio.h>
#include "JSON.h"

int main(int argc, const char * argv[]) {
    JSON* tmp=ParseJSONFromFile("f:/f.json");
    PrintJSONToFile(tmp,"f:/bbbb.json");
    getchar();
    return 0;
}
