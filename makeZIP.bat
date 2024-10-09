
REM ## make the exe .. it assumes EE-UUQ_Windows_Download exists with vcredist and openssl there



cd build
conan install .. --build missing
qmake QMAKE_CXXFLAGS+=-D_SC_RELEASE ..\EE-UQ.pro
nmake

REM ## copy application into folder and winddeployqt it

copy .\Release\EE_UQ.exe .\EE-UQ_Windows_Download
cd EE-UQ_Windows_Download
windeployqt EE_UQ.exe

REM ## copy examples

rmdir /s /q .\Examples
mkdir .\Examples
xcopy /s /e /Q ..\..\Examples .\Examples

REM ## delete applications folder and copy new stuff

rmdir /s /q .\applications
mkdir .\applications
mkdir .\applications\common
mkdir .\applications\createEDP
mkdir .\applications\createEVENT
mkdir .\applications\createSAM
mkdir .\applications\performSIMULATION
mkdir .\applications\performFEM
mkdir .\applications\performUQ
mkdir .\applications\Workflow
mkdir .\applications\opensees
mkdir .\applications\dakota
mkdir .\applications\python
mkdir .\applications\tools

xcopy /s /e ..\..\..\SimCenterBackendApplications\applications\common  .\applications\common
xcopy /s /e ..\..\..\SimCenterBackendApplications\applications\createEDP  .\applications\createEDP
xcopy /s /e ..\..\..\SimCenterBackendApplications\applications\createEVENT  .\applications\createEVENT
xcopy /s /e ..\..\..\SimCenterBackendApplications\applications\createSAM  .\applications\createSAM
xcopy /s /e ..\..\..\SimCenterBackendApplications\applications\performSIMULATION  .\applications\performSIMULATION
xcopy /s /e ..\..\..\SimCenterBackendApplications\applications\performFEM  .\applications\performFEM
xcopy /s /e ..\..\..\SimCenterBackendApplications\applications\performUQ  .\applications\performUQ
xcopy /s /e ..\..\..\SimCenterBackendApplications\applications\Workflow  .\applications\Workflow
xcopy /s /e ..\..\..\SimCenterBackendApplications\applications\OpenSees  .\applications\opensees
xcopy /s /e ..\..\..\SimCenterBackendApplications\applications\dakota  .\applications\dakota
xcopy /s /e /Q ..\..\..\SimCenterBackendApplications\applications\python  .\applications\python
xcopy /s /e  ..\..\..\SimCenterBackendApplications\applications\tools  .\applications\tools

rmdir /s /q .\applications\createEVENT\ASCE7_WindSpeed
rmdir /s /q .\applications\createEVENT\CFDEvent
rmdir /s /q .\applications\createEVENT\CFDEvent
rmdir /s /q .\applications\createEVENT\DEDM_HRP
rmdir /s /q .\applications\createEVENT\EmptyDomainCFD
rmdir /s /q .\applications\createEVENT\experimentalWindForces
rmdir /s /q .\applications\createEVENT\experimentalWindPressures
rmdir /s /q .\applications\createEVENT\GeoClawOpenFOAM
rmdir /s /q .\applications\createEVENT\HighRiseTPU
rmdir /s /q .\applications\createEVENT\IsolatedBuildingCFD
rmdir /s /q .\applications\createEVENT\LowRiseTPU
rmdir /s /q .\applications\createEVENT\MPM
rmdir /s /q .\applications\createEVENT\pointWindSpeed
rmdir /s /q .\applications\createEVENT\stochasticWind
rmdir /s /q .\applications\createEVENT\windTunnelExperiment



REM ## zip it up with 7zip

set sevenzip_path="C:\Program Files\7-Zip\7z.exe"
cd ..
%sevenzip_path% a -tzip .\EE-UQ_WindowsDownload.zip  .\EE-UQ_Windows_Download


