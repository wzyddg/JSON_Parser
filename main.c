//
//  main.c
//  JSONParser_14
//
//  Created by Ê© Ð¦³¿ on 14/12/7.
//  Copyright (c) 2014Äê Ê© Ð¦³¿. All rights reserved.
//

#include <stdio.h>
#include "JSON.h"

int main(int argc, const char * argv[]) {
    JSON* tmp=ParseJSONFromFile("f:/f.json");
    PrintJSONToFile(tmp,"f:/bbbb.json");
    getchar();
    return 0;
}
