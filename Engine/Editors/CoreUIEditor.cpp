//
//  CoreUIEditor.cpp
//  CoreEngine3D
//
//  Created by Jody McAdams on 7/14/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#if defined(_DEBUG_PC)

#include "CoreUIEditor.h"
#include "FLTKGLWindow.h"
#include "CoreObject_Manager.h"
#include "CoreObjectAttribute.h"

#include "CoreObjectAttribute_FLTK.h"

#include <sstream>

CoreUIEditor* UIEDITOR = NULL;

static const s32 UIEDITOR_WindowWidth = 400;

static const u32 g_UIEDITOR_HashedString_Unnamed = 2607469549; //Unnamed

static std::vector<CoreUI_Container> containers;

static void AttributeBrowserCallback (Fl_Widget *pWidget, void* pClassPointer)
{
	CoreUIEditor* pEditor = (CoreUIEditor*)pClassPointer;
	
	Fl_Tree* pTree = (Fl_Tree*)pWidget;
	
	switch(pTree->callback_reason())
	{
		case FL_TREE_REASON_NONE:
		{
			break;
		}
		case FL_TREE_REASON_SELECTED:
		{
			Fl_Tree_Item* pItem = pTree->callback_item();
			const unsigned long data = (unsigned long)pItem->user_data();
			const CoreObjectHandle handle = data;
			CoreUIView* pView = (CoreUIView*)COREOBJECTMANAGER->GetObjectByHandle(handle);
			
			if(pView == NULL)
			{
				break;
			}
			
			CoreObjectAttribute_Char32* pNameAttrib = (CoreObjectAttribute_Char32*)pView->attributes[pView->attrib_name];
			
			COREDEBUG_PrintMessage("Selected item: %s",(const char*)pNameAttrib->value);
			
			pEditor->DisplayAttributes(pView);
			
			pView->DEBUG_SetDebugVisible(true);
			
			break;
		}
		case FL_TREE_REASON_DESELECTED:
		{
			Fl_Tree_Item* pItem = pTree->callback_item();
			const unsigned long data = (unsigned long)pItem->user_data();
			const CoreObjectHandle handle = data;
			CoreUIView* pView = (CoreUIView*)COREOBJECTMANAGER->GetObjectByHandle(handle);
			
			if(pView == NULL)
			{
				break;
			}
			
			CoreObjectAttribute_Char32* pNameAttrib = (CoreObjectAttribute_Char32*)pView->attributes[pView->attrib_name];
			
			COREDEBUG_PrintMessage("Deselected item: %s",(const char*)pNameAttrib->value);

			pView->DEBUG_SetDebugVisible(false);
			
			break;
		}
		case FL_TREE_REASON_OPENED:
		{
			break;
		}
		case FL_TREE_REASON_CLOSED:
		{
			break;
		}
	}
}


void CoreUIEditor::DisplayAttributes(CoreUIView* pView)
{
	assert(pView != NULL);
	
	COREDEBUG_PrintMessage("Here be some attribs");
	
	m_attributeScrollView->clear();
	
	const s32 verticalSpacing = 16;
	const s32 horizontalSpacing = 16;
	
	const s32 widgetWidth = UIEDITOR_WindowWidth/1.5f;
	
	m_attributeYCurr = verticalSpacing;
	
	CoreObjectAttributeList& attribList = pView->attributes;
	
	const u32 numAttribs = attribList.numAttributes;
	for(u32 i=0; i<numAttribs; ++i)
	{
		CoreObjectAttribute* pAttrib = attribList[i];
		Fl_Widget* pWidget = CreateWidgetForAttribute(pAttrib,horizontalSpacing,m_attributeYCurr,widgetWidth,32,pView->GetHandle(),i);
		
		if(pWidget != NULL)
		{
			pWidget->align(FL_ALIGN_RIGHT);
			
			m_attributeScrollView->add(pWidget);
		
			m_attributeYCurr += pWidget->h()+24;
		}
	}
	
	m_attributeScrollView->scroll_to(0, -m_toolWindowBrowserScrollView->h());	//I think this is right :)
	m_attributeScrollView->redraw();
}


CoreUIEditor::CoreUIEditor()
{
	UIEDITOR = this;
	
	//Just a sanity check
	assert(MAINWINDOW != NULL);

	const s32 height = MAINWINDOW->h();
	const s32 halfHeight = height/2;
	
	//Create main tool window
	m_toolWindow = new Fl_Window( MAINWINDOW->x()+MAINWINDOW->w() + 32,0,UIEDITOR_WindowWidth,height,"UI Elements" );
	
	//Make window resizable
	m_toolWindow->resizable(m_toolWindow);
	
	//Everything past this will be added as a child of the main window
    m_toolWindow->begin();
	
	m_windowTile = new Fl_Tile(0,0,UIEDITOR_WindowWidth,height);
	
	{
		//Create a scroll view for the hierarchical object list
		m_toolWindowBrowserScrollView = new Fl_Scroll(0,0,UIEDITOR_WindowWidth,halfHeight,"uiscrollview");
		
		m_toolWindowBrowserScrollView->labeltype(FL_NO_LABEL);
		m_toolWindowBrowserScrollView->box(FL_DOWN_BOX);
		m_toolWindowBrowserScrollView->color((Fl_Color) FL_WHITE);
		m_toolWindowBrowserScrollView->type(Fl_Scroll::VERTICAL_ALWAYS);
		
		//Everything inside begin will be added as a child of m_toolWindowBrowserScrollView
		m_toolWindowBrowserScrollView->begin();
		
		{
			//Create the hierarchical object browser inside the scroll view
			m_toolWindowBrowser = new Fl_Tree(0,0,UIEDITOR_WindowWidth,halfHeight,"uibrowser");
			m_toolWindowBrowser->callback(AttributeBrowserCallback,this);
			m_toolWindowBrowser->box(FL_NO_BOX);
		}
		
		//End
		m_toolWindowBrowserScrollView->end();
		
		//Create a scroll view to hold the attribute list
		//It will be blank for now
		m_attributeScrollView = new Fl_Scroll(0,halfHeight,UIEDITOR_WindowWidth,halfHeight,"uiscrollview");
		
		m_attributeScrollView->box(FL_DOWN_BOX);
		m_attributeScrollView->labeltype(FL_NO_LABEL);
		m_attributeScrollView->color((Fl_Color) FL_WHITE);
		m_attributeScrollView->type(Fl_Scroll::VERTICAL_ALWAYS);
	}
	
	//End main window
    m_toolWindow->end();
	
	//We just popped up a window so it will take focus
	//We want to give focus back to the game so you don't lose control
	MAINWINDOW->take_focus();
	
	m_attributeYCurr = 0;
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
	std::stringstream appendedPath;
	appendedPath << path << "/";
	
	u32 defaultAppendNum = 0;
	
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
		
		std::stringstream itemPath;
		itemPath << appendedPath.str() << std::string((const char*)pNameAttrib->value);
		
		if(pNameAttrib->hashedValue == g_UIEDITOR_HashedString_Unnamed)
		{
			itemPath << defaultAppendNum;
			++defaultAppendNum;
		}
		
		Fl_Tree_Item* pItem = m_toolWindowBrowser->add(itemPath.str().c_str());
		pItem->user_data((void*)(u32)childObjectHandle);
		
		AddChildViews(pChildView, itemPath.str());
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
