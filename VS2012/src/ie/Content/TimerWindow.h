#pragma once
#include <comdef.h>

class CTimerWindow : public CWindowImpl<CTimerWindow, CWindow, CFrameWinTraits>
{
private:
	static HRESULT InvokeTimer(IDispatch* expression, DISPPARAMS* dp)
	{
		__try
		{
			return expression->Invoke(DISPID_VALUE, IID_NULL,
				LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, dp, NULL, NULL, NULL);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{

		}
		return E_FAIL;
	}

	BOOL m_bStopped;

public:
	DECLARE_WND_CLASS_EX(_T("CommonInterfaceTimerWindow"), 0, NULL);

	BEGIN_MSG_MAP(CTimerWindow)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
	END_MSG_MAP()

	class TimerCallback
	{
	public:
		TimerCallback(IDispatch* expression = NULL, BOOL continuous = NULL) : m_expression(expression), m_continuous(continuous)
		{

		}

		TimerCallback(const TimerCallback& other) : m_expression(other.m_expression), m_continuous(other.m_continuous)
		{

		}

		IDispatchPtr m_expression;
		BOOL m_continuous;

		HRESULT operator()()
		{
			DISPPARAMS dp = { 0 };
			return m_expression ? InvokeTimer(m_expression, &dp) : S_FALSE;
		}
	};

	CAtlMap<UINT_PTR, TimerCallback> m_timers;
	UINT_PTR m_nextTimer;

	CTimerWindow() : m_nextTimer(1), m_bStopped(FALSE)
	{

	}

	~CTimerWindow() 
	{
		m_bStopped = TRUE;
		m_timers.RemoveAll();
	}

	LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{

		UINT_PTR timerId = wParam;
		if (!m_bStopped && m_timers.Lookup(timerId))
		{
			TimerCallback timer = m_timers[timerId];
			if (!timer.m_continuous)
				RemoveTimer(timerId);
			timer();
// 			if (!timer.m_continuous) 
// 				m_timers.RemoveKey(timerId);
		}
		return 0;
	}

	STDMETHODIMP SetTimer(IDispatch* expression, long msec, long *pTimerId, BOOL continuous)
	{
		if (m_bStopped)
			return S_FALSE;
		if (!IsWindow())
		{
			ATLVERIFY(Create(NULL));
		}
		if (expression)
		{
			UINT_PTR timerId = CWindow::SetTimer(m_nextTimer++, msec);
			m_timers[timerId] = TimerCallback(expression, continuous);
			if (pTimerId)
				*pTimerId = timerId;
		}
		return S_OK;
	}

	STDMETHODIMP RemoveTimer(long timerId)
	{
		if (m_bStopped)
			return S_FALSE;
		CWindow::KillTimer(timerId);
		m_timers.RemoveKey(timerId);
		return S_OK;
	}

	HRESULT Stop()
	{
		m_bStopped = TRUE;
		m_timers.RemoveAll();
		return S_OK;
	}
};
