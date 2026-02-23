from copy import copy
from build import Build
import filecmp
import os, sys
import shutil
import unittest
# from dumi import Dumi 


class TestBuild(unittest.TestCase):

    def setUp(self) -> None:
        self.testFixtureDefineFilePath_MedipAlpha = "test_define_medip_alpha.h"
        self.testFixtureDefineFilePath_MedipBeta = "test_define_medip_beta.h"
        self.testFixtureDefineFilePath_MedipRelease = "test_define_medip_release.h"
        self.testFixtureDefineFilePath_DeepCatchAlpha = "test_define_deepcatch_alpha.h"
        self.testFixtureDefineFilePath_DeepCatchBeta = "test_define_deepcatch_beta.h"
        self.testFixtureDefineFilePath_DeepCatchRelease = "test_define_deepcatch_release.h"
        self.testDefineHeaderFile = "define_test.h"
        pass

    def tearDown(self) :
        if os.path.exists(Build.LicenseFileRelease) :
            os.remove(Build.LicenseFileRelease)

        if os.path.exists(self.testDefineHeaderFile) :
            os.remove(self.testDefineHeaderFile)

    def test_FileExist(self):
        
        self.assertTrue(os.path.exists(Build.LicenseDirectoryRelease))
        self.assertTrue(os.path.exists(Build.LicenseDirectoryRelease_Backup))
        self.assertTrue(os.path.exists(Build.LicenseFileRelease_MEDIP_Offline))
        self.assertTrue(os.path.exists(Build.LicenseFileRelease_MEDIP_Online))
        self.assertTrue(os.path.exists(Build.LicenseFileRelease_DeepCatch_Offline))
        self.assertTrue(os.path.exists(Build.LicenseFileRelease_DeepCatch_Online))

        self.assertTrue(os.path.exists(Build.LicenseDirectoryDebug))

        self.assertTrue(os.path.exists(Build.DefineHeaderTemplateFilePath))

        self.assertTrue(os.path.exists(self.testFixtureDefineFilePath_MedipAlpha))
        self.assertTrue(os.path.exists(self.testFixtureDefineFilePath_MedipBeta))
        self.assertTrue(os.path.exists(self.testFixtureDefineFilePath_MedipRelease))
        self.assertTrue(os.path.exists(self.testFixtureDefineFilePath_DeepCatchAlpha))
        self.assertTrue(os.path.exists(self.testFixtureDefineFilePath_DeepCatchBeta))
        self.assertTrue(os.path.exists(self.testFixtureDefineFilePath_DeepCatchRelease))

    def test_fileShouldBeEmpty(self):
        self.assertFalse(os.path.exists(Build.LicenseFileRelease))
        self.assertFalse(os.path.exists(self.testDefineHeaderFile))
        pass

    def test_copyLicenseFile_MEDIP(self):
        build = Build();
        build.copy_license_file("Release", "MEDIP", "Offline")
        self.assertTrue(os.path.exists(Build.LicenseFileRelease))
        self.assertTrue(filecmp.cmp(Build.LicenseFileRelease_MEDIP_Offline, Build.LicenseFileRelease))

        build.copy_license_file("Release", "MEDIP", "Online")
        self.assertTrue(os.path.exists(Build.LicenseFileRelease))
        self.assertTrue(filecmp.cmp(Build.LicenseFileRelease_MEDIP_Online, Build.LicenseFileRelease))
        pass

    def test_copyLicenseFile_DeepCatch(self):
        build = Build();
        build.copy_license_file("Release", "DeepCatch", "Offline")
        self.assertTrue(os.path.exists(Build.LicenseFileRelease))
        self.assertTrue(filecmp.cmp(Build.LicenseFileRelease_DeepCatch_Offline, Build.LicenseFileRelease))

        build.copy_license_file("Release", "DeepCatch", "Online")
        self.assertTrue(os.path.exists(Build.LicenseFileRelease))
        self.assertTrue(filecmp.cmp(Build.LicenseFileRelease_DeepCatch_Online, Build.LicenseFileRelease))
        pass

    def test_RunWithoutArgument_ReturnFalse(self):
        args = []
        build = Build()
        self.assertFalse(build.run(args))
        # print("hello world")
        pass

    def test_InvalidArguemt_ReturnFalse(self):
        args = ["build.py", "bad-command", "invalid", "bad"]
        build = Build()
        self.assertFalse(build.run(args))

    def test_copyArgument_MedipOnline(self):
        args = ["build.py", "copy", "medip", "online"]
        build = Build()
        self.assertTrue(build.run(args))
        self.assertTrue(os.path.exists(Build.LicenseFileRelease))
        self.assertTrue(filecmp.cmp(Build.LicenseFileRelease_MEDIP_Online, Build.LicenseFileRelease))

    def test_copyArgument_MedipOffline(self):
        args = ["build.py", "copy", "medip", "offline"]
        build = Build()
        self.assertTrue(build.run(args))
        self.assertTrue(os.path.exists(Build.LicenseFileRelease))
        self.assertTrue(filecmp.cmp(Build.LicenseFileRelease_MEDIP_Offline, Build.LicenseFileRelease))

    def test_copyArgument_DeepCatchOnline(self):
        args = ["build.py", "copy", "deepcatch", "online"]
        build = Build()
        self.assertTrue(build.run(args))
        self.assertTrue(os.path.exists(Build.LicenseFileRelease))
        self.assertTrue(filecmp.cmp(Build.LicenseFileRelease_DeepCatch_Online, Build.LicenseFileRelease))

    def test_copyArgument_DeepCatchOffline(self):
        args = ["build.py", "copy", "deepcatch", "offline"]
        build = Build()
        self.assertTrue(build.run(args))
        self.assertTrue(os.path.exists(Build.LicenseFileRelease))
        self.assertTrue(filecmp.cmp(Build.LicenseFileRelease_DeepCatch_Offline, Build.LicenseFileRelease))

    def test_updateDefineHeader(self):
        build = Build()
        build.update_define_header(self.testDefineHeaderFile, "MEDIP", "alpha")
        self.assertTrue(os.path.exists(self.testDefineHeaderFile))
        self.assertTrue(filecmp.cmp(self.testFixtureDefineFilePath_MedipAlpha, self.testDefineHeaderFile))

        build.update_define_header(self.testDefineHeaderFile, "MEDIP", "beta")
        self.assertTrue(os.path.exists(self.testDefineHeaderFile))
        self.assertTrue(filecmp.cmp(self.testFixtureDefineFilePath_MedipBeta, self.testDefineHeaderFile))

        build.update_define_header(self.testDefineHeaderFile, "MEDIP", "release")
        self.assertTrue(os.path.exists(self.testDefineHeaderFile))
        self.assertTrue(filecmp.cmp(self.testFixtureDefineFilePath_MedipRelease, self.testDefineHeaderFile))

        build.update_define_header(self.testDefineHeaderFile, "DeepCatch", "alpha")
        self.assertTrue(os.path.exists(self.testDefineHeaderFile))
        self.assertTrue(filecmp.cmp(self.testFixtureDefineFilePath_DeepCatchAlpha, self.testDefineHeaderFile))

        build.update_define_header(self.testDefineHeaderFile, "DeepCatch", "beta")
        self.assertTrue(os.path.exists(self.testDefineHeaderFile))
        self.assertTrue(filecmp.cmp(self.testFixtureDefineFilePath_DeepCatchBeta, self.testDefineHeaderFile))

        build.update_define_header(self.testDefineHeaderFile, "DeepCatch", "release")
        self.assertTrue(os.path.exists(self.testDefineHeaderFile))
        self.assertTrue(filecmp.cmp(self.testFixtureDefineFilePath_DeepCatchRelease, self.testDefineHeaderFile))
        pass

    def test_copyArgument_WithoutVersionOption_CopyReleaseDefineHeader(self):
        args = ["build.py", "copy", "medip", "offline"]
        build = Build()
        self.assertTrue(build.run(args))
        self.assertTrue(os.path.exists(Build.DefineHeaderFilePath))
        self.assertTrue(filecmp.cmp(self.testFixtureDefineFilePath_MedipRelease, Build.DefineHeaderFilePath))

    def test_copyArgument_InvalidVersionOption_CopyReleaseDefineHeader(self):
        args = ["build.py", "copy", "medip", "offline", "-v"]
        build = Build()
        self.assertTrue(build.run(args))
        self.assertTrue(os.path.exists(Build.DefineHeaderFilePath))
        self.assertTrue(filecmp.cmp(self.testFixtureDefineFilePath_MedipRelease, Build.DefineHeaderFilePath))

    def test_copyArgument_WithVersionOption_MedipAlpha(self):
        args = ["build.py", "copy", "medip", "offline", "-v", "alpha"]
        build = Build()
        self.assertTrue(build.run(args))
        self.assertTrue(os.path.exists(Build.DefineHeaderFilePath))
        self.assertTrue(filecmp.cmp(self.testFixtureDefineFilePath_MedipAlpha, Build.DefineHeaderFilePath))

    def test_copyArgument_WithVersionOption_MedipBeta(self):
        args = ["build.py", "copy", "medip", "offline", "-v", "beta"]
        build = Build()
        self.assertTrue(build.run(args))
        self.assertTrue(os.path.exists(Build.DefineHeaderFilePath))
        self.assertTrue(filecmp.cmp(self.testFixtureDefineFilePath_MedipBeta, Build.DefineHeaderFilePath))

    def test_copyArgument_WithVersionOption_MedipRelease(self):
        args = ["build.py", "copy", "medip", "offline", "-v", "release"]
        build = Build()
        self.assertTrue(build.run(args))
        self.assertTrue(os.path.exists(Build.DefineHeaderFilePath))
        self.assertTrue(filecmp.cmp(self.testFixtureDefineFilePath_MedipRelease, Build.DefineHeaderFilePath))

    def test_copyArgument_WithVersionOption_MedipAlpha(self):
        args = ["build.py", "copy", "deepcatch", "offline", "-v", "alpha"]
        build = Build()
        self.assertTrue(build.run(args))
        self.assertTrue(os.path.exists(Build.DefineHeaderFilePath))
        self.assertTrue(filecmp.cmp(self.testFixtureDefineFilePath_DeepCatchAlpha, Build.DefineHeaderFilePath))

    def test_copyArgument_WithVersionOption_MedipBeta(self):
        args = ["build.py", "copy", "deepcatch", "offline", "-v", "beta"]
        build = Build()
        self.assertTrue(build.run(args))
        self.assertTrue(os.path.exists(Build.DefineHeaderFilePath))
        self.assertTrue(filecmp.cmp(self.testFixtureDefineFilePath_DeepCatchBeta, Build.DefineHeaderFilePath))

    def test_copyArgument_WithVersionOption_MedipRelease(self):
        args = ["build.py", "copy", "deepcatch", "offline", "-v", "release"]
        build = Build()
        self.assertTrue(build.run(args))
        self.assertTrue(os.path.exists(Build.DefineHeaderFilePath))
        self.assertTrue(filecmp.cmp(self.testFixtureDefineFilePath_DeepCatchRelease, Build.DefineHeaderFilePath))

if __name__ == '__main__':
    unittest.main()