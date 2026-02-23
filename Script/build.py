import sys, os
import shutil

AUTH_METHOD_ONLINE = "online"
AUTH_METHOD_OFFLINE = "offline"

PRODUCT_MEDIP = "medip"
PRODUCT_DEEPCATCH = "deepcatch"

CONFIG_DEBUG = "debug"
CONFIG_RELEASE = "release"

VERSION_ALPHA = "alpha"
VERSION_BETA = "beta"
VERSION_RELEASE = "release"

COMMAND_COPY = "copy"

class Build:
    def __init__(self):
        self.root_directory = "../Output/x64"
        self.LicenseDirectoryRelease = "../Output/x64/Release/license"
        self.LicenseDirectoryRelease_Backup = self.LicenseDirectoryRelease + "/medipVersionInfo_backup"
        self.LicenseFileRelease = self.LicenseDirectoryRelease + "/medipVersionInfo.dat"
        self.LicenseFileRelease_MEDIP_Offline = self.LicenseDirectoryRelease_Backup + "/medipVersionInfo_offline_medip.dat"
        self.LicenseFileRelease_MEDIP_Online = self.LicenseDirectoryRelease_Backup + "/medipVersionInfo_online_medip.dat"
        self.LicenseFileRelease_DeepCatch_Offline = self.LicenseDirectoryRelease_Backup + "/medipVersionInfo_offline_deepcatch.dat"
        self.LicenseFileRelease_DeepCatch_Online = self.LicenseDirectoryRelease_Backup + "/medipVersionInfo_online_deepcatch.dat"

        self.LicenseDirectoryDebug = "../Output/x64/Debug/license"

        self.DefineHeaderDirectoryPath = "../MedipQT"
        #DefineHeaderFilePath = DefineHeaderDirectoryPath + "/define_new.h"
        self.DefineHeaderFilePath = self.DefineHeaderDirectoryPath + "/define.h"
        self.DefineHeaderTemplateFilePath = self.DefineHeaderDirectoryPath + "/define.h.tmpl"

        self.product = PRODUCT_MEDIP
        self.auth_method = AUTH_METHOD_ONLINE
        self.config = CONFIG_RELEASE
        self.version = VERSION_RELEASE

    def print_usage(self) :
        print("python build.py [command] [args]")
        print("\tcopy - build.py copy [config] [product] [auth method]")
        print("\t\t- config(debug,release), product(medip, deepcatch), auth method(online, offline)")

    def parse(self, args):
        pass

    def run(self, args):
        count = len(args)
        if count < 5:
            self.print_usage()
            return False

        filePath = args.pop(0)
        command = args.pop(0)
        config_arg = args.pop(0)
        product_arg = args.pop(0)
        auth_method_arg = args.pop(0)

        if command == COMMAND_COPY :
            print("start run license copy process")

            if config_arg == CONFIG_DEBUG  :
                self.config = CONFIG_DEBUG
            elif config_arg == CONFIG_RELEASE :
                self.config = CONFIG_RELEASE
            else :
                print("invalid config arg : " + config_arg)
                self.print_usage()
                return False

            if product_arg == PRODUCT_MEDIP  :
                self.product = PRODUCT_MEDIP
            elif product_arg == PRODUCT_DEEPCATCH :
                self.product = PRODUCT_DEEPCATCH
            else :
                print("invalid product : " + product_arg)
                self.print_usage()
                return False
            
            if auth_method_arg == AUTH_METHOD_ONLINE :
                self.auth_method = AUTH_METHOD_ONLINE
            elif auth_method_arg == AUTH_METHOD_OFFLINE :
                self.auth_method = AUTH_METHOD_OFFLINE
            else :
                print("invalid auth method : " + auth_method_arg)
                return False
            
            if self.copy_license_file( self.config, self.product, self.auth_method) == False :
                print("fail to copy license file")
                return False
            
            #if versionType != "release" :
            #    print("copy define header file : " + Build.DefineHeaderFilePath)
            #    if self.updateDefineHeader(Build.DefineHeaderFilePath, self.product, versionType) == False :
            #        print("fail to update define header")
            #        return False
        else :
            print("invalid command : " + command)
            return False
        
        return True

    def copy_license_file(self, config, product, auth_method):
        license_source_directory = ''
        license_dest_file_path = ''
        if config == CONFIG_DEBUG:
            license_source_directory = os.path.join(self.root_directory, 'Debug/license/medipVersionInfo_backup')
            license_dest_file_path = os.path.join(self.root_directory, 'Debug/license/medipVersionInfo.dat')
        elif config == CONFIG_RELEASE:
            license_source_directory = os.path.join(self.root_directory, 'Release/license/medipVersionInfo_backup')
            license_dest_file_path = os.path.join(self.root_directory, 'Release/license/medipVersionInfo.dat')
        
        license_file_name = 'medipVersionInfo'

        if auth_method == AUTH_METHOD_ONLINE:
            license_file_name += '_online'
        elif auth_method == AUTH_METHOD_OFFLINE:
            license_file_name += '_offline'

        if product == PRODUCT_DEEPCATCH:
            license_file_name += '_deepcatch'
        elif product == PRODUCT_MEDIP:
            license_file_name += '_medip'

        license_file_name += '.dat'

        license_file_path = os.path.join(license_source_directory, license_file_name)

        shutil.copyfile(license_file_path, license_dest_file_path)
        return True

    def update_define_header(self, headerFilePath, product, versionType):
        fileRead = open(Build.DefineHeaderTemplateFilePath, 'r')
        fileWrite = open(headerFilePath, 'w')

        useMedip = "0"
        useDeepCatch = "0"
        useAlphaVersion = "0"
        useBetaVersion = "0"

        if product == "MEDIP":
            useMedip = "1"
            useDeepCatch = "0"
        elif product == "DeepCatch":
            useMedip = "0"
            useDeepCatch = "1"
        else:
            print("fail to update define header. invalid product : " + product) 
            return False

        if versionType == "alpha":
            useAlphaVersion = "1"
            useBetaVersion = "0"
        elif versionType == "beta":
            useAlphaVersion = "0"
            useBetaVersion = "1"
        elif versionType == "release":
            useAlphaVersion = "0"
            useBetaVersion = "0"
        else :
            useAlphaVersion = "0"
            useBetaVersion = "0"
        
        keyValueDitcionary = {
            "py_use_medip": useMedip,
            "py_use_deepcatch": useDeepCatch,
            "py_use_alpha_version": useAlphaVersion,
            "py_use_beta_version": useBetaVersion,
        }

        readTemplateData = fileRead.read()
        resultData = readTemplateData % keyValueDitcionary

        fileWrite.write(resultData)

        fileRead.close()
        fileWrite.close()

        # shutil.copyfile(Build.DefineHeaderTemplateFilePath, headerFilePath)
        pass


if __name__ == '__main__':
    arguments = sys.argv
    #print("data" + argument)
    build = Build()

    #arguments = ["build.py", "copy", "debug", "medip", "offline"]
    #arguments = ["build.py", "copy", "debug", "medip", "online"]
    #arguments = ["build.py", "copy", "release", "medip", "offline"]
    #arguments = ["build.py", "copy", "release", "medip", "online"]

    #arguments = ["build.py", "copy", "debug", "deepcatch", "offline"]
    #arguments = ["build.py", "copy", "debug", "deepcatch", "online"]
    #arguments = ["build.py", "copy", "release", "deepcatch", "offline"]
    #arguments = ["build.py", "copy", "release", "deepcatch", "online"]

    build.run(arguments)
