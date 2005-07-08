#pragma once

#include <list>

class CTreeCtrlNodeMemento
{
public:
	CTreeCtrlNodeMemento(CTreeCtrlNode node);
	virtual ~CTreeCtrlNodeMemento(void);

	void Restore(CTreeCtrlNode node);
	void Dump(int nIndent = 0);
private:
	CString                          m_text;
	UINT                             m_state;
	std::list<CTreeCtrlNodeMemento*> m_children;
};

inline CTreeCtrlNodeMemento::CTreeCtrlNodeMemento(CTreeCtrlNode node)
{
	this->m_text = node.GetText();
	this->m_state = node.GetState(0xFFFF);
	for (int i = 0; i < node.GetChildCount(); ++i)
	{
		this->m_children.push_back(new CTreeCtrlNodeMemento(node.GetChildAt(i)));
	}
}

inline CTreeCtrlNodeMemento::~CTreeCtrlNodeMemento(void)
{
	std::list<CTreeCtrlNodeMemento*>::reverse_iterator it = this->m_children.rbegin();
	for (; it != m_children.rend(); ++it)
	{
		delete (*it);
	}
	this->m_children.clear();
}

inline void CTreeCtrlNodeMemento::Restore(CTreeCtrlNode node)
{
	std::list<CTreeCtrlNodeMemento*>::iterator it = this->m_children.begin();
	for (; it != this->m_children.end(); ++it)
	{
		(*it)->Restore(node.AddTail((*it)->m_text));
	}
	node.SetText(this->m_text);
	// the Expand must be sent before setting the state otherwise
	// the vertical scrollbar behaves badly
	// and the treectrl stops redrawing the bottom of the rectangle
	if (this->m_state & TVIS_EXPANDED) node.Expand(TVE_EXPAND);
	node.SetState(this->m_state, 0xFFFF);
}

inline void CTreeCtrlNodeMemento::Dump(int nIndent)
{
	// indent using spaces
	for (int i = 0; i < nIndent; ++i)
	{
		::OutputDebugString(_T("  "));
	}
	if (!this->m_children.empty())
	{
		// [+] or [-] only if node contains children
		::OutputDebugString((this->m_state & TVIS_EXPANDED) ? _T("[-]") : _T("[+]"));
	}
	// output text
	::OutputDebugString((LPCTSTR)this->m_text);
	::OutputDebugString(_T("\n"));

	if (!(this->m_state & TVIS_EXPANDED)) return;  // skip children

	std::list<CTreeCtrlNodeMemento*>::iterator it = this->m_children.begin();
	for (; it != this->m_children.end(); ++it)
	{
		(*it)->Dump(nIndent + 1);
	}
}


class CTreeMemento
{
public:
	CTreeMemento(CTreeCtrlNode node);
	virtual ~CTreeMemento(void);

	CTreeCtrlNode Restore(void);
private:
	int           m_nIndex;
	CTreeCtrlNode m_nodeParent;
	CTreeCtrlNode m_top;
private:
	CTreeCtrlNodeMemento         m_node;
};

inline CTreeMemento::CTreeMemento(CTreeCtrlNode node)
: m_nIndex(node.GetParent().GetIndex(node))
, m_nodeParent(node.GetParent())
, m_top(node.GetFirstVisible())
, m_node(node)
{
	ASSERT(this->m_nIndex >= 0);
#if defined(_DEBUG)
	::OutputDebugString(_T("First Visible: "));
	::OutputDebugString((LPCTSTR)m_top.GetText());
	::OutputDebugString(_T("\n"));
#endif
}

inline CTreeMemento::~CTreeMemento()
{
}

inline CTreeCtrlNode CTreeMemento::Restore(void)
{
	ASSERT(this->m_nIndex >= 0);

	if (this->m_nIndex > 0)
	{
		CTreeCtrlNode prevSibling = this->m_nodeParent.GetChildAt(this->m_nIndex - 1);
		ASSERT(prevSibling && prevSibling.GetParent() == this->m_nodeParent);

		CTreeCtrlNode node = this->m_nodeParent.InsertAfter(_T(""), prevSibling);
		this->m_node.Restore(node);
		this->m_top.SelectSetFirstVisible();
		return node;
	}

	CTreeCtrlNode node = this->m_nodeParent.AddHead(_T(""));
	this->m_node.Restore(node);
	this->m_top.SelectSetFirstVisible();
	return node;
}
