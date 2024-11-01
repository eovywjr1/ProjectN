import unreal
import pandas as pd
import os
from typing import Dict, List, Tuple

excelPath = unreal.SystemLibrary.get_project_directory() + "Data/DataExcel"
csvPath = unreal.SystemLibrary.get_project_directory() + "Data/DataCSV"

findPropertyColumnIndex = 0

class UnrealExcelConverter:
    def __init__(self):
        self.editor_asset_lib = unreal.EditorAssetLibrary()
        
        self.type_mapping = {
            'int': 'int32',
            'string': 'FString',
            'list:int': 'TArray<int32>',
            'list:string': 'TArray<FString>'
        }

    def loadExcel(self, xlsx):
        file_path = excelPath + "/" + xlsx
         
        try:
            allSheet = pd.read_excel(file_path, sheet_name=None)
        except Exception as e:
            unreal.log_error(f"엑셀 파일 읽기 실패: {str(e)}")
            return
        
        for sheetName, excelData in allSheet.items():
            if excelData.empty:
                continue

            unreal.log_error(f"{sheetName} 워크시트 Convert Start")

            columnTypes = {}

            try:
                first_column = excelData.iloc[:, 0].tolist()

                propertyMask = excelData.iloc[:, 0] == "Property"

                if not propertyMask.any():
                    unreal.log_error(f"{xlsx} 파일의 {sheetName} 워크시트에 {findPropertyColumnIndex} 행에 Property 값이 없습니다. 수정해주세요")
                    return

                propertyRows = excelData[propertyMask].index[0]

                propertyData = excelData.iloc[propertyRows, 1:]

                typeData = excelData.iloc[propertyRows - 1, 1:].values

                for property, type in zip(propertyData, typeData):
                    ueType = self.type_mapping.get(str(type))
                    if property.startswith("_") == False and ueType is None:
                        unreal.log_error(f"{xlsx} 파일에 Type: {type} 은 없습니다. int, string, list:int, list:string만 입력해주세요.")
                        return

                    columnTypes[property] = ueType

                self.convertExcelToCSV(xlsx, sheetName, excelData, columnTypes)

            except Exception as e:
                unreal.log_error(f"처리 중 에러 발생: {str(e)}")

    def convertExcelToCSV(self, xlsx, sheetName, excelData: pd.DataFrame, columnTypes: Dict[str, str]):
        csvFilePath = f"{csvPath}/{sheetName}.csv"
        if os.path.exists(csvFilePath):
           try:
               os.remove(csvFilePath)
           except Exception as e:
               unreal.log_error(f"Failed to delete CSV file: {str(e)}")
               return
   
        properties = []
        exceptPropertyRowIndex = []

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
            return
            
        propertyRowIndex = excelData[propertyMask].index[0]
        startRowIndex = propertyRowIndex + 1
        startColIndex = findPropertyColumnIndex + 1

        for rowIndex in range(startRowIndex, len(excelData)):
            rowData = []
                
            for colIndex in range(startColIndex, len(excelData.columns)):
                if colIndex in exceptPropertyRowIndex:
                    continue

                value = excelData.iloc[rowIndex, colIndex]
                if pd.isna(value):
                    continue
                
                colType = columnTypes.get(excelData.iloc[propertyRowIndex, colIndex])
                
                if 'TArray' in colType:
                    if ';' in str(value):
                        values = str(value).split(';')
                        if 'int32' in colType:
                            value = f"({','.join(values)})"
                        else:
                            values = [f'""{v}""' for v in values]
                            value = f"({','.join(values)})"

                        value = f'"{value}"'

                rowData.append(str(value))
            
            csvData.append(','.join(rowData))

        
        with open(csvFilePath, 'w', encoding='utf-8') as f:
            f.write('\n'.join(csvData))

        self.createDatatable(xlsx, sheetName)

    def createDatatable(self, xlsx: str, sheetName):
        try:
            
            
            datatablePath = f"/Game/ProjectN/DataTable/DT_{sheetName}"
            
            if self.editor_asset_lib.does_asset_exist(datatablePath):
                self.editor_asset_lib.delete_asset(datatablePath)

            headerPath = f"/Script/ProjectN.{sheetName}"
            loadedStruct = unreal.load_object(None, headerPath)
            if not loadedStruct:
                return
            
            datatableFactory = unreal.CSVImportFactory()
            datatableFactory.automated_import_settings.import_row_struct = loadedStruct
            
            task = unreal.AssetImportTask()
            task.filename = f"{csvPath}/{sheetName}.csv"
            task.destination_path = "/Game/ProjectN/DataTable"
            task.destination_name = f"DT_{sheetName}"
            task.automated = True
            task.save = True
            task.factory = datatableFactory

            unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
            
        except Exception as e:
            unreal.log_error(f"Fatal error creating DataTable: {str(e)}")
            unreal.log_error(f"Error type: {type(e).__name__}")

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
                self.loadExcel(xlsx)
            
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