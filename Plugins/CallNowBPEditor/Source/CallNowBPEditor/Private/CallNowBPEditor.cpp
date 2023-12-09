/*
Copyright (c) 2023 hyaniner@outlook.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "CallNowBPEditor.h"

#include "BlueprintEditorModule.h"
#include "GraphEditor.h"
#include "SGraphPanel.h"
#include "SGraphPin.h"

#define LOCTEXT_NAMESPACE "FCallNowBPEditorModule"


class FPinFactoryTestCommands : public TCommands<FPinFactoryTestCommands>
{
public:

	FPinFactoryTestCommands()
		: TCommands<FPinFactoryTestCommands>
		(
			TEXT("FPinFactoryTestCommands")
			, LOCTEXT("FPinFactoryTestCommands", "FPinFactoryTestCommands")
			, NAME_None
			, FAppStyle::Get().GetStyleSetName()
		)
	{}

	// TCommands<> interface
	virtual void RegisterCommands() override;

	TSharedPtr< FUICommandInfo > SetPinFactory;
	
};

void FPinFactoryTestCommands::RegisterCommands()
{
	UI_COMMAND(SetPinFactory, "SetPinFactory", "SetPinFactory", EUserInterfaceActionType::Button, FInputChord());
}

TSharedPtr<SGraphPin> FTestPinFactory::CreatePinWidget(UEdGraphPin* InPin)
{
	if(InPin)
	{
		UE_LOG(LogTemp, Warning, TEXT("FTestPinFactory Passed!!!: NodeName : %s PinName : %s"), *(InPin->GetOwningNode()->GetNodeTitle(ENodeTitleType::FullTitle).ToString()), *(InPin->GetDisplayName().ToString()));	
	}
	TSharedPtr<SGraphPin> Result = FGraphNodeFactory::CreatePinWidget(InPin);
	if(Result.IsValid())
	{
		//Make pins to pink to identify them
		Result->SetColorAndOpacity(FLinearColor(1.0f, 0.5f, 0.7f));		
	}	
	return Result;
}

void FCallNowBPEditorModule::StartupModule()
{
	FCoreDelegates::OnFEngineLoopInitComplete.AddRaw(this, &FCallNowBPEditorModule::OnFEngineLoopInitComplete);	
}

void FCallNowBPEditorModule::ShutdownModule()
{
	FPinFactoryTestCommands::Unregister();
	TestCommands.Reset();
	Factory.Reset();
}


void FCallNowBPEditorModule::OnFEngineLoopInitComplete()
{
	FPinFactoryTestCommands::Register();
	
	TestCommands = MakeShareable(new FUICommandList);
	TestCommands->MapAction(
		FPinFactoryTestCommands::Get().SetPinFactory
		, FExecuteAction::CreateRaw(this, &FCallNowBPEditorModule::SetPinFactory)
		, FCanExecuteAction()
		);
	
	FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
	BlueprintEditorModule.GetsSharedBlueprintEditorCommands()->Append(TestCommands.ToSharedRef());
	TSharedPtr<FExtensibilityManager> ExtensibilityManager = BlueprintEditorModule.GetMenuExtensibilityManager();
	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
	MenuExtender->AddMenuBarExtension("Tools", EExtensionHook::After, TestCommands, FMenuBarExtensionDelegate::CreateRaw(this, &FCallNowBPEditorModule::MakePullDownMenuBlueprintEditor));
	ExtensibilityManager->AddExtender(MenuExtender);

	Factory = MakeShareable(new FTestPinFactory());	
}

void FCallNowBPEditorModule::SetPinFactory()
{
	UE_LOG(LogTemp, Warning, TEXT("Begin of SetPinFactory Test Function"));
	
	const TSharedRef<FGlobalTabmanager> TabManager = FGlobalTabmanager::Get(); 
	const TSharedPtr<SDockTab> ActiveTab = TabManager->GetActiveTab();
	if(ActiveTab.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("ActiveTab Found(Title : %s)"), *ActiveTab->GetTabLabel().ToString());
		
		const TSharedRef<SWidget> TabContent = ActiveTab->GetContent();
		if(TabContent->GetTypeAsString() == FString(TEXT("SGraphEditor")))
		{
			
			TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(TabContent);
			FString Identity = GraphEditor->GetType().ToString();			 
			UE_LOG(LogTemp, Warning, TEXT("Active SGraphEditor TabFound(Type:%s), Now trying to set the test factory."), *Identity);
			
			GraphEditor->SetNodeFactory(Factory.ToSharedRef());

			//Update the pins that existed before setting the factory
			UEdGraph* Graph = GraphEditor->GetCurrentGraph();
			Graph->NotifyGraphChanged();
			
			UE_LOG(LogTemp, Warning, TEXT("Setting factory is done. If it worked well, you can see the color of the pins and its text turned to pink. The test finished as success."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ActiveTab was not SGraphEditor. Try again. Click carefully in order: graph tab, and menu."));
			
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can not find the ActiveTab. Try again. Click carefully in order: graph tab, and menu."));
	}
	
	UE_LOG(LogTemp, Warning, TEXT("End of SetPinFactory Test Function"));
}

void FCallNowBPEditorModule::MakePullDownMenuBlueprintEditor(FMenuBarBuilder& menuBuilder)
{
	menuBuilder.AddPullDownMenu(
	LOCTEXT("SetPinFactory", "SetPinFactory"),
	LOCTEXT("SetPinFactory", "SetPinFactory"),
	FNewMenuDelegate::CreateRaw(this, &FCallNowBPEditorModule::FillPullDownMenuBlueprintEditor),
	"FCallNowBPEditorModule"	
	);
}

void FCallNowBPEditorModule::FillPullDownMenuBlueprintEditor(FMenuBuilder& menuBuilder)
{
	menuBuilder.BeginSection("BPPatternFindReplaceMenuSection");
	
	menuBuilder.AddMenuEntry(
		FPinFactoryTestCommands::Get().SetPinFactory		
		, TEXT("FPinFactoryTestModuleHookMenu")
		, LOCTEXT("SetPinFactory", "SetPinFactory")
		, LOCTEXT("SetPinFactory", "SetPinFactory")
		, FSlateIcon()
		);

	menuBuilder.EndSection();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCallNowBPEditorModule, CallNowBPEditor)