# Where to create and save a fuzzing test case harness

It is recommended to create fuzzing test harness cases in the file structure provided in HBFA-FL. At a minimum, the folder used for the fuzzing test cases should be located in a sub-directory under the 'HBFA' directory for use with the RunAFL.py and RunLibFuzzer.py script. For a fuzzing test harness, a minimal test case folder would consist of the test harness C-source code file (containing the fuzzing harness logic) and a module description file (the .inf file). The 'HBFA' folder shown in the file structure (tree) relative to the HBFA-FL repository root directory.

```
├── HBFA
    ├── UefiHostFuzzTestCasePkb
        ├── TestCase
            ├── ...                    # The directories here somewhat follow the 
                DeviceSecurityPkg      #  analogous directories from the EDK2 root
                FatPkg                 #  file structure (e.g. see MdeModulePkg)
                MdeModulePkg                     
                OvmfPkg
                SecurityPkg
                ...
                    └── TestModuleFolder   # This is the folder containing the new test case
                        └── TestXYZ.c      # Test harness/logic goes in this C-source code file
                        └── TestXYZ.inf    # The INF, module description file

```

[&lt;&lt;](../fuzzing/README.md) Back | Return to [Summary](../SUMMARY.md) | Next [&gt;&gt;](../fuzzing/building.md)