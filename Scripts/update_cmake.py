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
        print("Missing params: python update_cmake.py <project_name> <project_sub_path>")
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

    print("Update cmake: %s at %s" % (project_name, project_path))
    
    # Create Project folder
    if (os.path.exists(project_path) is False):
        print("Warning: Project %s isnot exist" % (project_path))
        return

    target_cmake = project_path + "/CMakeLists.txt"
    shutil.copy("Scripts/CMakeLists.txt", target_cmake)
    replace_text(target_cmake, "@project_path@", project_path)
    replace_text(target_cmake, "@project_name@", project_name)
    
    target_cmake = project_path + "/CMakeProject.cmake"
    shutil.copy("Scripts/CMakeProject.cmake", target_cmake)

if __name__ == '__main__':
    main()
