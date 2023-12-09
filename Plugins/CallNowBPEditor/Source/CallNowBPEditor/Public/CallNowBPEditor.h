// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"
#include "NodeFactory.h"
#include "Modules/ModuleManager.h"


class FTestPinFactory : public FGraphNodeFactory
{
public:
	virtual ~FTestPinFactory(){}
	virtual TSharedPtr<SGraphPin> CreatePinWidget(UEdGraphPin* InPin) override;
};


class FCallNowBPEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	void OnFEngineLoopInitComplete();

	void SetPinFactory();
	void MakePullDownMenuBlueprintEditor(FMenuBarBuilder &menuBuilder);
	void FillPullDownMenuBlueprintEditor(FMenuBuilder &menuBuilder);

	TSharedPtr<class FUICommandList> TestCommands;
	TSharedPtr<FTestPinFactory> Factory;
};
