import sys
import os
import shutil

def replace_text(file, find, replace):
    f = open(file, 'r')
    filedata = f.read()
    f.close()

    newdata = filedata.replace(find, replace)

    f = open(file, 'w')
    f.write(newdata)
    f.close()


def main():
    # Check param
    if len(sys.argv) < 3:
        print("Missing params: python update_version.py <project_name> <project_sub_path>")
        return

    # Check current folder
    if (os.path.exists("Scripts/CMakeLists.txt") is False):
        print("- Error: Must run script at root skylicht engine folder")
        return

    # Parse params
    project_name = sys.argv[1]
    project_path = sys.argv[2]

    project_name = project_name.replace(' ', '_')
    project_path = project_path.replace('\\', '/')

    print("Update version project: %s at %s" % (project_name, project_path))

    copy_source = True

    # Create Project folder
    if (os.path.exists(project_path) is False):
        os.makedirs(project_path + "/Source")

    with open('Version.txt') as f:
        version = f.read().replace('\n', '')

        version_h = project_path + "/Source/Version.h"
        shutil.copy("Scripts/Version.h", version_h)
        replace_text(version_h, "@project_name@", project_name)
        replace_text(version_h, "@project_version@", version)


if __name__ == '__main__':
    main()
