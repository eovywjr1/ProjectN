import unreal
import pandas as pd
import os
from typing import Dict, List, Tuple

ProjectDir = unreal.SystemLibrary.get_project_directory()
ExcelDir = ProjectDir + "Data/DataExcel"
CSVDir = ProjectDir + "Data/DataCSV"

PropertyRowIdentifier = "Property"
PropertyColumnIndex = 0

UnrealTypeMapping = {
            'int': 'int32',
            'string': 'FString',
            'list:int': 'TArray<int32>',
            'list:string': 'TArray<FString>'
}

UnrealEditorAssetLib = unreal.EditorAssetLibrary()

def ProcessConvertExcel() -> bool:
    FileList = os.listdir(ExcelDir)
    ExcelFileList = []

    for File in FileList:
        if File.endswith(".xlsx"):
            ExcelFileList.append(File)

    LodeadExcelAndWorkSheetNameList = []
    
    for ExcelFileNameAndExtension in ExcelFileList:
        if(ExcelFileNameAndExtension.startswith("~")):
            continue

        unreal.log_error(f"{ExcelFileNameAndExtension} Convert Start")

        try:
            ExcelFilePath = ExcelDir + "/" + ExcelFileNameAndExtension
            LoadedExcel = pd.read_excel(ExcelFilePath, sheet_name=None)

        except Exception as e:
            unreal.log_error(f"엑셀 파일({ExcelFileNameAndExtension}) 읽기 실패: {str(e)}")
            return False

        for SheetName, SheetData in LoadedExcel.items():
            if SheetData.empty:
                continue

            # SheetName 중복 체크
            for LoadedFileName, LoadedSheetName in LodeadExcelAndWorkSheetNameList:
                if LoadedSheetName == SheetName:
                    unreal.log_error(f"{LoadedFileName} - {LoadedSheetName} 과 {ExcelFileNameAndExtension} - {SheetName}이 중복됩니다. 워크시트 이름을 수정해주세요")
                    return False

            LodeadExcelAndWorkSheetNameList.append((ExcelFileNameAndExtension, SheetName))

            unreal.log_error(f"{SheetName} 워크시트 Convert Start")

            try:
                ConvertExcelToCSV(ExcelFileNameAndExtension, SheetName, SheetData)

                ConvertCSVToDataTable(SheetName)

            except Exception as e:
                unreal.log_error(f"처리 중 에러 발생 {ExcelFileNameAndExtension}: {str(e)}")
                return False
        
        unreal.log_error(f"{ExcelFileNameAndExtension} Convert End")

def ConvertExcelToCSV(ExcelFileNameAndExtension, SheetName, SheetData: pd.DataFrame):
    PropertyMask = SheetData.iloc[:, 0] == PropertyRowIdentifier
    if not PropertyMask.any():
        unreal.log_error(f"{ExcelFileNameAndExtension} 파일의 {SheetName} 워크시트에 {PropertyColumnIndex} 행에 Property 값이 없습니다. 수정해주세요")
        return

    PropertyRowIndex = SheetData[PropertyMask].index[0]
    TypeRowIndex = PropertyRowIndex - 1
    PropertyRowDataList = SheetData.iloc[PropertyRowIndex, 1:]

    LoadPropertyList = []
    ExceptPropertyColumnIndexList = []

    for ColumnIndex in range(len(PropertyRowDataList)):
        if PropertyRowDataList[ColumnIndex].startswith("_"):
            ExceptPropertyColumnIndexList.append(ColumnIndex + 1)
        else:
            LoadPropertyList.append(PropertyRowDataList[ColumnIndex])
    
    CSVData = []
    CSVData.append(','.join(LoadPropertyList))
        
    StartRowIndex = PropertyRowIndex + 1
    StartColIndex = PropertyColumnIndex + 1

    for RowIndex in range(StartRowIndex, len(SheetData)):
        RowData = []
            
        for ColIndex in range(StartColIndex, len(SheetData.columns)):
            if ColIndex in ExceptPropertyColumnIndexList:
                continue

            Value = SheetData.iloc[RowIndex, ColIndex]
            if pd.isna(Value):
                continue
            
            Type = UnrealTypeMapping.get(str(SheetData.iloc[TypeRowIndex, ColIndex]))
            
            if 'TArray' in Type and ';' in str(Value):
                TempValue = str(Value).split(';')

                if 'int32' in Type:
                    Value = f"({','.join(TempValue)})"
                elif 'FString' in Type:
                    TempValue = [f'""{v}""' for v in TempValue]
                    Value = f"({','.join(TempValue)})"

                Value = f'"{Value}"'

            RowData.append(str(Value))
        
        CSVData.append(','.join(RowData))

    CSVFilePath = f"{CSVDir}/{SheetName}.csv"
    if os.path.exists(CSVFilePath):
       try:
           os.remove(CSVFilePath)
       except Exception as e:
           unreal.log_error(f"Failed to delete CSV file: {str(e)}")
           return
    
    with open(CSVFilePath, 'w', encoding='utf-8') as f:
        f.write('\n'.join(CSVData))

def ConvertCSVToDataTable(SheetName):
    try:
        DataTableDir = "/Game/ProjectN/DataTable"
        DataTablePath = DataTableDir + f"/DT_{SheetName}"
        
        if UnrealEditorAssetLib.does_asset_exist(DataTablePath):
            UnrealEditorAssetLib.delete_asset(DataTablePath)

        DataTableStructHeaderPath = f"/Script/ProjectN.{SheetName}"
        DataTableStructObject = unreal.load_object(None, DataTableStructHeaderPath)
        if not DataTableStructObject:
            unreal.log_error(f"해당 데이터 테이블 구조체가 없습니다. 클라에게 요청해주세요 {DataTableStructHeaderPath}")
            return
        
        DatatableFactory = unreal.CSVImportFactory()
        DatatableFactory.automated_import_settings.import_row_struct = DataTableStructObject
        
        DataTableTask = unreal.AssetImportTask()
        DataTableTask.filename = f"{CSVDir}/{SheetName}.csv"
        DataTableTask.destination_path = DataTableDir
        DataTableTask.destination_name = f"DT_{SheetName}"
        DataTableTask.automated = True
        DataTableTask.save = True
        DataTableTask.factory = DatatableFactory
        unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([DataTableTask])
        
    except Exception as e:
        unreal.log_error(f"Fatal error creating DataTable: {str(e)}")
        unreal.log_error(f"Error type: {type(e).__name__}")

def main():
    success = ProcessConvertExcel()
    
    if success:
        print("Conversion completed successfully")
    else:
        print("Conversion failed")

if __name__ == "__main__":
    main()