import unreal
import pandas as pd
import os
from typing import Dict, List, Tuple

projectName = "ProjectN"
excelPath = unreal.SystemLibrary.get_project_directory() + "Data/DataExcel"
csvPath = unreal.SystemLibrary.get_project_directory() + "Data/DataCSV"

findPropertyColumnIndex = 0

class UnrealExcelConverter:
    def __init__(self):
        self.asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
        self.editor_asset_lib = unreal.EditorAssetLibrary()
        
        self.type_mapping = {
            'int': 'int32',
            'string': 'FString',
            'list:int': 'TArray<int32>',
            'list:string': 'TArray<FString>'
        }

    def processExcel(self, xlsx) -> Tuple[pd.DataFrame, Dict[str, str]]:
        # 1. 파일 경로 확인
        file_path = excelPath + "/" + xlsx
        unreal.log_error(f"1. 읽을 파일 경로: {file_path}")
         
        try:
            excelData = pd.read_excel(file_path, header=None)
            unreal.log_error("2. 엑셀 파일 읽기 성공")
        except Exception as e:
            unreal.log_error(f"2. 엑셀 파일 읽기 실패: {str(e)}")
            return None, []
        
        columnTypes = {}
        
        try:
            first_column = excelData.iloc[:, 0].tolist()
            unreal.log_error(f"3. A열 전체 데이터: {first_column}")

            propertyMask = excelData.iloc[:, 0] == "Property"
            unreal.log_error(f"4. Property 마스크: {propertyMask}")
        
            if not propertyMask.any():
                unreal.log_error(f"5. {xlsx} 파일에 {findPropertyColumnIndex} 행에 Property 값이 없습니다. 수정해주세요")
                return None, []
             
            propertyRows = excelData[propertyMask].index[0]
            unreal.log_error(f"6. Property 행 번호: {propertyRows}")
             
            propertyData = excelData.iloc[propertyRows, 1:]
            unreal.log_error(f"7. Property 데이터: {propertyData.tolist()}")
             
            typeData = excelData.iloc[propertyRows - 1, 1:].values
            unreal.log_error(f"8. Type 데이터: {typeData.tolist()}")
        
            for property, type in zip(propertyData, typeData):
                ueType = self.type_mapping.get(str(type))
                unreal.log_error(f"9. 처리중: property={property}, type={type}, ueType={ueType}")
        
                # if ueType is None:
                #     unreal.log_error(f"10. {xlsx} 파일에 Type: {type} 은 없습니다. int, string, list:int, list:string만 입력해주세요.")
                #     return None, []
                 
                columnTypes[property] = ueType
        
            unreal.log_error(f"11. 최종 컬럼 타입: {columnTypes}")
            return excelData, columnTypes
        
        except Exception as e:
            unreal.log_error(f"처리 중 에러 발생: {str(e)}")
            return None, []

    def convertExcelToCSV(self, xlsx, excelData: pd.DataFrame, columnTypes: Dict[str, str]) -> bool:
        csvFilePath = f"{csvPath}/{os.path.splitext(xlsx)[0]}.csv"

        # 기존 CSV 파일이 있는지 확인하고 삭제
        if os.path.exists(csvFilePath):
           try:
               os.remove(csvFilePath)
               unreal.log_error(f"Existing CSV file deleted: {csvFilePath}")
           except Exception as e:
               unreal.log_error(f"Failed to delete CSV file: {str(e)}")
               return False
   
        properties = []
        exceptPropertyRowIndex = []

        unreal.log_error(f"{columnTypes}")
        for columnTypeIndex, (key, value) in enumerate(columnTypes.items()):
            if key.startswith("_"):
                exceptPropertyRowIndex.append(columnTypeIndex + 1)
            else:
                properties.append(key)

                        
        csvData = []
        csvData.append(','.join(properties))

        propertyColumn = excelData.columns[findPropertyColumnIndex]
        propertyMask = excelData[propertyColumn].astype(str).eq('Property')
        if not propertyMask.values.any():
            unreal.log_error("9. Property row not found!")
            return False
            
        propertyRowIndex = excelData[propertyMask].index[0]
        startRowIndex = propertyRowIndex + 1
        startColIndex = findPropertyColumnIndex + 1

        unreal.log_error(len(excelData))
        unreal.log_error(len(excelData.columns))
        for rowIndex in range(startRowIndex, len(excelData)):
            rowData = []
                
            for colIndex in range(startColIndex, len(excelData.columns)):
                if colIndex in exceptPropertyRowIndex:
                    continue

                value = excelData.iloc[rowIndex, colIndex]
                if pd.isna(value):
                    continue
                
                # 타입에 따른 처리
                colType = columnTypes.get(excelData.iloc[propertyRowIndex, colIndex])
                unreal.log_error(columnTypes)
                if 'TArray' in colType:  # TArray 타입인 경우
                    if ';' in str(value):
                        values = str(value).split(';')
                        if 'int32' in colType:
                            # 정수 배열
                            value = f"({','.join(values)})"
                        else:
                            # 문자열 배열
                            values = [f'""{v}""' for v in values]  # 각 문자열을 따옴표로 감싸기
                            unreal.log_error(len(values))
                            value = f"({','.join(values)})"

                        value = f'"{value}"'

                rowData.append(str(value))
            
            csvData.append(','.join(rowData))

        unreal.log_error(f"{csvData}")
        with open(f"{csvPath}/{os.path.splitext(xlsx)[0]}.csv", 'w', encoding='utf-8') as f:
            f.write('\n'.join(csvData))

        return True

    def createDatatable(self, xlsx: str) -> bool:
        try:
            xlsxName = os.path.splitext(xlsx)[0]
            datatablePath = f"/Game/ProjectN/DataTable/DT_{xlsxName}"
            unreal.log_error(datatablePath)
            unreal.log_error(f"1. Creating DataTable for {xlsxName}")
            unreal.log_error(f"3. Datatable Path: {datatablePath}")
            
            if self.editor_asset_lib.does_asset_exist(datatablePath):
                unreal.log_error("4. Deleting existing asset...")
                self.editor_asset_lib.delete_asset(datatablePath)

            headerPath = f"/Script/ProjectN.{os.path.splitext(xlsx)[0]}"
            loadedStruct = unreal.load_object(None, headerPath)
            if not loadedStruct:
                unreal.log_error(f"5. Failed to load struct from path: {headerPath}")
                return False
            
            unreal.log_error("6. Creating DataTable asset...")

            datatableFactory = unreal.CSVImportFactory()
            datatableFactory.automated_import_settings.import_row_struct = loadedStruct
            
            task = unreal.AssetImportTask()
            task.filename = f"{csvPath}/{xlsxName}.csv"
            task.destination_path = "/Game/ProjectN/DataTable"  # 에셋 저장 경로
            task.destination_name = f"DT_{xlsxName}"  # 데이터테이블 이름
            task.automated = True  # 자동 임포트
            task.save = True  # 임포트 후 저장
            task.factory = datatableFactory

            unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])

            return True
            
        except Exception as e:
            unreal.log_error(f"Fatal error creating DataTable: {str(e)}")
            unreal.log_error(f"Error type: {type(e).__name__}")
            return False

    def convertExcelToDatatable(self) -> bool:
        fileList = os.listdir(excelPath)
        xlsxFileList = []

        for file in fileList:
            if file.endswith(".xlsx"):
                xlsxFileList.append(file)
        
        for xlsx in xlsxFileList:
            if(xlsx.startswith("~")):
                continue

            unreal.log_error(f"{xlsx} Convert Start")

            try:
                excelData, columnTypes = self.processExcel(xlsx)
                if(excelData.empty):
                    return False
                
                self.convertExcelToCSV(xlsx, excelData, columnTypes)
                
                self.createDatatable(xlsx)
            
            except Exception as e:
                unreal.log_error(f"Conversion failed: {str(e)}")
                return False
            
            unreal.log_error(f"{xlsx} Convert End")
            
        return True

def main():
    converter = UnrealExcelConverter()
    success = converter.convertExcelToDatatable()
    
    if success:
        print("Conversion completed successfully")
    else:
        print("Conversion failed")

if __name__ == "__main__":
    main()