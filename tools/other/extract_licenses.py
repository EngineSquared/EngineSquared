import subprocess
import shutil
from typing import Optional

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

class Package:
    def __init__(self):
        self.name: Optional[str] = None
        self.license: Optional[str] = None

    def load(self, string: str):
        parsed: dict[str, str] = self.__parse(string)
        self.name = parsed.get("name")
        self.license = parsed.get("license")

    def __parse(self, string: str) -> dict[str, str]:
        result: dict[str, str] = {}
        result["name"] = string.split(" ")[0].split(")")[0]
        attribute_lines = string.split("\n")[1:]
        for attribute_line in attribute_lines:
            if attribute_line.find("license") != -1:
                result["license"] = attribute_line.split(": ")[1]
        return result



def main():
    xmake = shutil.which("xmake")
    if xmake is None:
        raise FileNotFoundError("xmake executable not found in PATH")

    subprocess.run([xmake, "g", "--theme=plain"], capture_output=True, text=True, check=True)
    output = subprocess.run([xmake, "q", "-l"], capture_output=True, text=True, check=True)
    splited: list[str] = output.stdout.split("require(")[1:]
    packages: dict[Optional[str], Package] = {}
    for block in splited:
        package = Package()
        package.load(block)
        packages[package.name if package.name is not None else "Invalid"] = package
    max_package_name_size: int = 0
    for package_name in packages.keys():
        if package_name == None:
            continue
        package_name_size = len(package_name)
        if max_package_name_size < package_name_size:
            max_package_name_size = package_name_size
    for package in packages.values():
        if package.license is not None:
            print(f"Package: {{package_name:<{max_package_name_size}}} license: {package.license}".format(package_name=package.name))
        else:
            print(f"{bcolors.FAIL}Package: {{package_name:<{max_package_name_size}}} license: {package.license}{bcolors.ENDC}".format(package_name=package.name))

if __name__ == "__main__":
    main()
