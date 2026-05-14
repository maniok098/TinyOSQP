%% Clean up previous directories
if isfolder('build')
    rmdir('build', 's');
end
if isfolder('out')
    rmdir('out', 's');
end

%% Build static lib with CMake
!cmake -B build
!cmake --build build -j --config Release
!cmake --install build --prefix out

%% Compile MEX files
if ismac
    error('Unsupported platform');
elseif isunix
    %  Linux
    % Unset LD_LIBRARY_PATH to avoid conflicts, if any
    setenv('LD_LIBRARY_PATH','')
    mex -v mex_example_osqp.c -Iout/include out/lib/libosqp.a 
elseif ispc
    % Windows 
    mex -v mex_example_osqp.c -Iout/include out/lib/osqp.lib
else
    disp('Platform not supported')
end

%% Run example
mex_example_osqp;