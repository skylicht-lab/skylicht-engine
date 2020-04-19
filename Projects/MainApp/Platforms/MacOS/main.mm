//
//  main.m
//  mainapp
//
//  Created by Tuyet Nguyet on 4/11/20.
//  Copyright © 2020 Skylicht Co LTD. All rights reserved.
//

#include "AngleApplication.h"

int main(int argc, const char * argv[]) {    
    AngleApplication app(std::string("Skylicht Engine"), argc, (char**)argv, 3, 0);
    return app.run();
}
