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

#include "CoreObjectAttribute_FLTK.h"

CoreUIEditor* UIEDITOR = NULL;

static const s32 UIEDITOR_WindowWidth = 256;

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
			
			CoreObjectAttribute_Char32* pNameAttrib = (CoreObjectAttribute_Char32*)pView->attributes[pView->attrib_name];
			
			COREDEBUG_PrintMessage("Selected item: %s",(const char*)pNameAttrib->value);
			
			pEditor->DisplayAttributes(pView->attributes);
			
			break;
		}
		case FL_TREE_REASON_DESELECTED:
		{
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

void CoreUIEditor::DisplayAttributes(CoreObjectAttributeList& attribList)
{
	COREDEBUG_PrintMessage("Here be some attribs");
	
	m_attributeScrollView->clear();
	
	m_attributeYCurr = 0;
	
	const u32 numAttribs = attribList.numAttributes;
	for(u32 i=0; i<numAttribs; ++i)
	{
		CoreObjectAttribute* pAttrib = attribList[i];
		Fl_Widget* pWidget = CreateWidgetForAttribute(pAttrib,m_attributeYCurr,UIEDITOR_WindowWidth);
		
		if(pWidget != NULL)
		{
			m_attributeScrollView->add(pWidget);
		
			m_attributeYCurr += pWidget->h();
		}
	}
	
	/*for(u32 i=0; i<2; ++i)
	{
		Fl_Text_Display* pLabel = new Fl_Text_Display(0,i*32,UIEDITOR_WindowWidth,32,"hi");
		m_attributeScrollView->add(pLabel);
	}*/
	
	m_attributeScrollView->redraw();
}


CoreUIEditor::CoreUIEditor()
{
	UIEDITOR = this;
	
	//Just a sanity check
	assert(MAINWINDOW != NULL);

	const s32 height = MAINWINDOW->h();
	
	m_toolWindow = new Fl_Window( MAINWINDOW->x()+MAINWINDOW->w() + 32,0,UIEDITOR_WindowWidth,height,"UI Elements" );

	
    m_toolWindow->begin();
	
	const s32 halfHeight = height/2;
	
	m_toolWindowBrowser = new Fl_Tree(0,0,UIEDITOR_WindowWidth,halfHeight,"uibrowser");
	m_toolWindowBrowser->callback(AttributeBrowserCallback,this);
	
	m_attributeScrollView = new Fl_Scroll(0,halfHeight,UIEDITOR_WindowWidth,halfHeight,"uiscrollview");
	
	m_attributeScrollView->box(FL_DOWN_BOX);
	m_attributeScrollView->color((Fl_Color) FL_WHITE);
	m_attributeScrollView->type(Fl_Scroll::VERTICAL);
	
    m_toolWindow->end();
	
	MAINWINDOW->take_focus();
	
	/*for(u32 i=0; i<32; ++i)
	{
		Fl_Text_Display* pLabel = new Fl_Text_Display(0,i*32,width,32,"hi");
		m_attributeScrollView->add(pLabel);
	}*/
	
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
