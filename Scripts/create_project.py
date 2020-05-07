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

    # Create Project folder
    if (os.path.exists(project_path) is True):
        print("- Error: %s is exits" % (project_path))
        return
    else:
        os.makedirs(project_path + "/Source")

    target_cmake = project_path + "/CMakeLists.txt"
    shutil.copy("Scripts/CMakeLists.txt", target_cmake)
    replace_text(target_cmake, "@project_path@", project_path)
    replace_text(target_cmake, "@project_name@", project_name)


if __name__ == '__main__':
    main()
