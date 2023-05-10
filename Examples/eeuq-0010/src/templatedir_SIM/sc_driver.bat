"C:/Users/SimCenter/Sangri/SimCenterBackendApplications/applications/performUQ/templateSub/simCenterSub.exe" params.in AIM.json.sc AIM.json
"C:/Users/SimCenter/Sangri/SimCenterBackendApplications/applications/performUQ/templateSub/simCenterSub.exe" params.in EVENT.json.sc EVENT.json
call ./driver.bat

"C:/Users/SimCenter/Sangri/SimCenterBackendApplications/applications/performUQ/common/extractEDP" EDP.json results.out
