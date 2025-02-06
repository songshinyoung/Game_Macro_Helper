//---------------------------------------------------------------------------

#ifndef MCommonObjectH
#define MCommonObjectH
//---------------------------------------------------------------------------

#include <Windows.h>

//////////////////////////////////////////////////////////
class TFrequencyTimer
{
private:

    // PerformanceCounter �� PerformanceFrequency ������ ������ �ʴ����� ��� �ð��� ���� �� �ִ�.
    LARGE_INTEGER m_liFrequency;        // clock ���ļ� ���ϴ� ����
    LARGE_INTEGER m_liPerfCounter;      // PC�� On �� �� ���� ���� ������ clock ���� ���ϴ� ����

    bool    m_bStarted;         //  Timer ���� ��Ȳ
    UINT    m_nLimitTime;       //  ���� : msec
    double  m_dwStartTime;      //  ���� : msec
    double  m_dwCrntTime;       //  ���� : msec
    double  m_dwStartTimeSec;   //  ���� : sec.

    UINT    m_nDelayTime;
    double  m_dwStartDelayTime;

public:
    TFrequencyTimer()
    {
        QueryPerformanceFrequency(&m_liFrequency);
        m_bStarted = false;
        m_nLimitTime  = 0;
        m_dwStartTime = 0;
        m_dwCrntTime  = 0;
        m_dwStartTimeSec = 0;
        m_dwStartDelayTime = 0;
        m_nDelayTime = 0;
    };

    //-----------------------------------------------------------------
    void StartDelay(UINT nDelayMSEC)
    {
        QueryPerformanceCounter(&m_liPerfCounter);
        m_nDelayTime        = nDelayMSEC;
		m_dwStartDelayTime  = (((double)(m_liPerfCounter.QuadPart * 1000.0) / (double)(m_liFrequency.QuadPart)));
		m_bStarted			= true;
    }

    bool IsDelayEnd()
    {
        QueryPerformanceCounter(&m_liPerfCounter);
        double m_dwCrntTimeDelay = (((double)(m_liPerfCounter.QuadPart * 1000.0) / (double)(m_liFrequency.QuadPart))); //

        if((m_dwCrntTimeDelay - m_dwStartDelayTime) > m_nDelayTime)
		{
			m_bStarted = false;
            return true;
        }

        return false;
    }

    //-----------------------------------------------------------------

    double  StartTimer(UINT nTimeOutMSEC)   // timer�� �����Ѵ�. (���� �ð��� msec������ �����Ѵ�)
    {
        QueryPerformanceCounter(&m_liPerfCounter);
        m_bStarted       = true;
        m_nLimitTime     = nTimeOutMSEC;
        m_dwStartTime    = (((double)(m_liPerfCounter.QuadPart * 1000.0) / (double)(m_liFrequency.QuadPart)));
        m_dwStartTimeSec = ((double)(m_liPerfCounter.QuadPart / (double)(m_liFrequency.QuadPart)));
        return m_dwStartTime;
    };

    double  StartTimerSEC(UINT nTimeOutSEC)   // timer�� �����Ѵ�. (���� �ð��� sec ������ ���� �Ѵ�.)
    {
        QueryPerformanceCounter(&m_liPerfCounter);
        m_bStarted       = true;
        m_nLimitTime     = nTimeOutSEC * 1000.0;
        m_dwStartTime    = (((double)(m_liPerfCounter.QuadPart * 1000.0) / (double)(m_liFrequency.QuadPart)));
        m_dwStartTimeSec = ((double)(m_liPerfCounter.QuadPart / (double)(m_liFrequency.QuadPart)));
        return m_dwStartTimeSec;
    };

    double  GetLapTimeMM()              // msec ������ ��� �ð��� ���´�.
    {
        QueryPerformanceCounter(&m_liPerfCounter);
        m_dwCrntTime = (((double)(m_liPerfCounter.QuadPart * 1000.0) / (double)(m_liFrequency.QuadPart))); //

        return (m_dwCrntTime - m_dwStartTime);
    };

    double  GetLapTimeMM(double dStartTime)              // msec ������ ��� �ð��� ���´�.
    {
        QueryPerformanceCounter(&m_liPerfCounter);
        m_dwCrntTime = (((double)(m_liPerfCounter.QuadPart * 1000.0) / (double)(m_liFrequency.QuadPart))); //

        return (m_dwCrntTime - dStartTime);
    };

    double  GetLapTimeSEC()             // sec ������ ��� �ð��� ���´�.
    {
        QueryPerformanceCounter(&m_liPerfCounter);
        m_dwCrntTime = (((double)(m_liPerfCounter.QuadPart) / (double)(m_liFrequency.QuadPart))); //

        return (m_dwCrntTime - m_dwStartTimeSec);
    };

    double  GetLapTimeSEC(double dStartTimeSec)             // sec ������ ��� �ð��� ���´�.
    {
        QueryPerformanceCounter(&m_liPerfCounter);
        m_dwCrntTime = (((double)(m_liPerfCounter.QuadPart) / (double)(m_liFrequency.QuadPart))); //

        return (m_dwCrntTime - dStartTimeSec);
    };


    bool    IsTimeOut()                 // timeout �Ǿ����� Ȯ���Ѵ�.
    {
        QueryPerformanceCounter(&m_liPerfCounter);
        m_dwCrntTime = (((double)(m_liPerfCounter.QuadPart * 1000.0) / (double)(m_liFrequency.QuadPart))); //

        if((m_dwCrntTime - m_dwStartTime) > m_nLimitTime)
        {
            m_bStarted       = false;
            return true;
        }

        return false;
    };

    bool    IsTimeOut(double dStartTime)                 // timeout �Ǿ����� Ȯ���Ѵ�.
    {
        QueryPerformanceCounter(&m_liPerfCounter);
        m_dwCrntTime = (((double)(m_liPerfCounter.QuadPart * 1000.0) / (double)(m_liFrequency.QuadPart))); //

        if((m_dwCrntTime - dStartTime) > m_nLimitTime)
        {
            m_bStarted       = false;
            return true;
        }

        return false;
    };

    void    StopTimer()
    {
        m_bStarted = false;
    };

    bool    IsStarted()
    {
        return m_bStarted;
    };

    bool    IsStopped()
    {
        return !m_bStarted;
    };

};



#endif

