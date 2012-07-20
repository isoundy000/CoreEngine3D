//
//  CoreUIEditor.cpp
//  CoreEngine3D(OSX)
//
//  Created by Jody McAdams on 7/14/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#if defined(_DEBUG_PC)

#include "CoreUIEditor.h"
#include "FLTKGLWindow.h"
#include "CoreObject_Manager.h"
#include "CoreObjectAttribute.h"

CoreUIEditor* UIEDITOR = NULL;

static std::vector<CoreUI_Container> containers;

CoreUIEditor::CoreUIEditor()
{
	UIEDITOR = this;
	
	//Just a sanity check
	assert(MAINWINDOW != NULL);

	const s32 width = 256;
	const s32 height = MAINWINDOW->h();
	
	m_toolWindow = new Fl_Window( MAINWINDOW->x()+MAINWINDOW->w() + 32,0,width,height,"UI Elements" );

	
    m_toolWindow->begin();
	
	const s32 halfHeight = height/2;
	
	m_toolWindowBrowser = new Fl_Tree(0,0,width,halfHeight,"uibrowser");
	
	m_attributeScrollView = new Fl_Scroll(0,halfHeight,width,halfHeight,"uiscrollview");
	
	m_attributeScrollView->box(FL_DOWN_BOX);
	m_attributeScrollView->color((Fl_Color) FL_WHITE);
	m_attributeScrollView->type(Fl_Scroll::VERTICAL);
	m_attributeScrollView->begin();
	
	m_attributeScrollView->end();
	
    m_toolWindow->end();
	
	MAINWINDOW->take_focus();
	
	for(u32 i=0; i<32; ++i)
	{
		Fl_Text_Display* pLabel = new Fl_Text_Display(0,i*32,width,32,"hi");
		m_attributeScrollView->add(pLabel);
	}
}


CoreUIEditor::~CoreUIEditor()
{
	delete m_toolWindow;
}


void CoreUIEditor::Clear()
{
	m_toolWindowBrowser->clear();
}


void CoreUIEditor::SetVisible(bool isVisible)
{
	if(isVisible)
	{
		m_toolWindow->show();
		
		MAINWINDOW->take_focus();
	}
	else
	{
		m_toolWindow->hide();
	}
}


void CoreUIEditor::AddChildViews(CoreUIView* pParentView,const std::string& path)
{
	const std::string appendedPath = path + '/';
	const std::string defaultPath = appendedPath+"Unnamed";
	std::string itemPath;
	
	for(u32 i=0; i<pParentView->numChildren; ++i)
	{
		const CoreObjectHandle  childObjectHandle = pParentView->children[i];
		CoreUIView* pChildView = (CoreUIView*)COREOBJECTMANAGER->GetObjectByHandle(childObjectHandle);
		if(pChildView == NULL)
		{
			continue;
		}
		
		//TODO: should use cache to look up
		CoreObjectAttribute_Char32* pNameAttrib = (CoreObjectAttribute_Char32*)pChildView->attributes.GetAttributeByName("name");
		
		itemPath = appendedPath+std::string((const char*)pNameAttrib->value);
		
		Fl_Tree_Item* pItem = m_toolWindowBrowser->add(itemPath.c_str());
		pItem->user_data((void*)(u32)childObjectHandle);
		
		AddChildViews(pChildView, itemPath);
	}
}


void CoreUIEditor::AddViewContainer(const CoreUI_Container& container)
{
	containers.push_back(container);
	
	const CoreObjectHandle parentObjectHandle = container.rootView;
	CoreUIView* pParentView = (CoreUIView*)COREOBJECTMANAGER->GetObjectByHandle(parentObjectHandle);
		 
	//TODO: should use cache to look up
	CoreObjectAttribute_Char32* pNameAttrib = (CoreObjectAttribute_Char32*)pParentView->attributes.GetAttributeByName("name");

	std::string itemPath = std::string((const char*)pNameAttrib->value);

	Fl_Tree_Item* pItem = m_toolWindowBrowser->add(itemPath.c_str());
	pItem->user_data((void*)(u32)parentObjectHandle);
	
	AddChildViews(pParentView,itemPath);
}

#endif
