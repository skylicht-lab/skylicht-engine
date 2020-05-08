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
        print("Missing params: python create_project.py <project_name> <project_sub_path>")
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

    print("Create project: %s at %s" % (project_name, project_path))

    copy_source = True

    # Create Project folder
    if (os.path.exists(project_path) is True):
        print("Warning: Project %s is exits" % (project_path))
        answer = raw_input(
            "Do use want override CMakeLists.txt - Yes[y] or No[n]: ")
        if answer != "y":
            return
        else:
            print("Override!")
            copy_source = False
    else:
        os.makedirs(project_path + "/Source")

    target_cmake = project_path + "/CMakeLists.txt"
    shutil.copy("Scripts/CMakeLists.txt", target_cmake)
    replace_text(target_cmake, "@project_path@", project_path)
    replace_text(target_cmake, "@project_name@", project_name)

    if copy_source == True:
        source_h = project_path + "/Source/" + project_name + ".h"
        source_cpp = project_path + "/Source/" + project_name + ".cpp"
        shutil.copy("Scripts/Template.h", source_h)
        replace_text(source_h, "@project_name@", project_name)

        shutil.copy("Scripts/Template.cpp", source_cpp)
        replace_text(source_cpp, "@project_name@", project_name)


if __name__ == '__main__':
    main()
