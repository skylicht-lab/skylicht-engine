//
//  main.m
//  mainapp
//
//  Created by Tuyet Nguyet on 4/11/20.
//  Copyright © 2020 Skylicht Co LTD. All rights reserved.
//

#include "SampleApplication.h"

int main(int argc, const char * argv[]) {
    SampleApplication app(std::string("Skylicht Engine"), argc, (char**)argv);
    return app.run();
}
