## How to implement numerical solver in TwinCAT
### As C++ TcCOM object

1. Create TwinCAT certificate
    1.1 TwinCAT -> software protection -> neu erstellen
    -  OEM Name:  xxxx(.tccert)
    -  Eindeutiger Name:  some name e.g. Email adress
    -  Zertifikat für :  alle Optionen auswählen 
    
    1.2 START
    - Speichern (notice: the folder )
    - passwort:  e.g. schnitzel
    - ok  

2. Create new C++ projekt
    - C++ neues Element
    - versioned C++ project (change the Name !)
       - create C++ program as callable interface within PLC
    - TwinCAT Module Class  (determine the call of function in ST with call)
       
3. Tc sign for C++ project
    - right click C++ project  
    - Eigenschaft >> Tc Sign >> 
       - TwinCAT signing -> ja
       - TwinCAT certificate 
            - Name (xxxx.tccert) (notice:  with .tccert !!S)
            - passwort  

<!-- 4. Create TcCOM object
   - Right click, (one level higher) projekt
   - neues Element hinzufügen 
   - create TcCOM objekt -->

5. Add the certificate for this C++ TcCOM manually to trusted list
   - C:\TwinCAT\3.1\Target\OemCertificates\xxxx
   - select the certificate >> double click >> ja ja ja

6. Configuration aktivieren

7. Close the default filter of C++ project to add source code 
   - C++ projekt -> Alle Dateien anzeigen (toolbar on the top of explorer) (suche in Schnellstart)
   - Get rid of the filter to copy source code of optimizer

8. set IOs of TcCOM object under projektname.tmc (double click)
   - Data Types
   - Add new interface (to call as function block in PLC)
       - Data Types >> Interface  <!-- >>persistent (to be seen in PLC)-->
   - Add methods (functions)
   - Add parameters (define IOs of the method) 
      Inputs of method: 
      - if only scalar value  -> Type LREAL,  Description:  Normal Type
      - if array  -> Type LREAL,  Description: Is Pointer

      Output of method: 
      - if only scalar vale  -> type LREAL, Description: Is Reference
      -  if array  -> Type LREAL,  description: Is Pointer

9. Add new interface 
    - .tmc file >> Modules >> Implemented Interfaces >> Add new interface

10. Run TMC Code Generation  
    - also within .tmc file viewer
    - top left, green button

11. Implement function in C++
    - ModulName.cpp  --> write function
    - Notice the variables:  with *  : pointer, with & : reference
    - Use [ ] to explicitly read and write the values of pointer and reference

    Important:
    - make sure, while using pointer for IOs, do not exceed the dimension 

12. In PLC :  copy or create a function block as interface TcCOM
      - see template FB file
      - Add FB_init and FB_exit (No necessary change from template)
      - Define ip (interface pointer) and iid (interface id) according to TcCOM object

13. Call the function block in PLC program
     - Use LREAL double for IOs 
     - Do not use REAL

14. PLC projekt erstellen 
     - Do not directly configuration aktivieren

15. Ddd TcCOM objekt to PLC instance now
    - Under PLC Instance (double click) 
    - Symbol Initialisierung 
    - Wert auswählen (add TcCOM Objekt), do not choose PlcTask
    - Notice: If you change the name of the function block instance, you have to do the symbol initialization again under PLC instance !!

16. Configuration aktivieren

### Add GRAMPC in C++ projekt


1. Copy source code of the solver to C++ project
    -  GRAMPC_v2.2 (modified source code of solver for TwinCAT)  
    -  And MPC_PI_Ctrl (problem specific file of user) 
    -  To folder FiveX\GRAMPC (C++ project folder)
2. Add folder "include" and "src" to project (zu Projekt hinzufügen)
3. Add Problem specification file to project  (must rename to .cpp, not .c)
    
4. modify .h file of the entire cpp project, add at the beginning 
    ```cpp
      #include "GRAMPC_v2.2/include/grampc.h"
    ```
5. modify probfct_xxxxx.cpp  file of specification file for solver
    ```cpp
      #include "../TcPch.h"
      #pragma hdrstop
      #include "../GRAMPC_v2.2/include/probfct.h"
    ```
6. Define the problem to be solved in probfct_xxxxx.cpp
    - modify the dimensions accordingly in \include\fixedsize_setting.h

7. In projectmain.cpp, e.g. Numerical_solver.cpp
    - Initialization of parameters under SetObjStatePS
       - copy the code of template
    - Write main part of cyclic computation 

8. Define  init values of MPC in projectname.h
    - only define values, not passed to grampc yet
    - under protected custome variables

9. Pass init values to grampc in projectname.cpp
    -  SetObjStatePS

10. Options for solver in fixed_size_setting.h
    - e.g. MAXGRADITER usw..

11. Run grampc and update values in projectname.cpp
    - change return to HRESULT hr = S_OK;

12. Modify the explicit line search in grampc_run to avoid zero dividing 


### Add OSQP in C++ projekt
1. Copy source code of osqp and qdldl to the TwinCAT c++ project folder 
    - See for example the folder `standalone_sovlers\osqp`
    - Notice: Do not do the copy in TwinCAT environment, but in the windows explorer
    - After copying, add the folders to the project in TwinCAT environment by refreshing the project
    - Add the source code of the solver to the project in TwinCAT (add to project, or zu Projekt hinzufügen)
2.  Add the include path to twincat cpp project
    -  Right click on the project -> Eigenschaften -> C/C++ -> Allgemein -> Additional Include Directories
    -  Add the path to the include folder of the solver
    -  Refer to `IncludeDirs.png` for the path

3. refer to `Cpp_OSQP_Cyclic_Update_vector` for the implementation of OSQP in TwinCAT
    -  add corresponding implementation in header file and cpp file
    -  deactive the polishing of osqp to save time
    -  change the max_iter etc. in the solver

4. Update cpp source then apply changes
    - First, project neu erstellen
    - Then, reload tmc file under TcCOM object
    - Finally, configuration aktivieren




### Common Errors/Features: 
- System ist in einem ungültigen Zustand (Logo in yellow)
   - First go to config modus 
   - Then configuration aktivieren

- Debug of C++, e.g. Divide by zero in algorithm
   - Double click C++ project, C++ debug aktivieren
   - switch from release to debug mode  ( if no drop down options for debug mode,  right click on toolbar -> standard)
   - Start debug in TwinCAT 

- How to add headers to the path
   - Right click on the project -> Eigenschaften -> C/C++ -> Allgemein -> Additional Include Directories
   - Add the path to the include folder of the solver
   - Refer to `IncludeDirs.png` for the path

- If header include path has been added to the project, but still not found
   - restart TwinCAT

- Ensure all methods in cpp returns S_OK

- Select Auto Neuladen TMI/TMC