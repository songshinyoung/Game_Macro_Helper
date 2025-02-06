//---------------------------------------------------------------------------

#ifndef MCommonObjectH
#define MCommonObjectH
//---------------------------------------------------------------------------

#include <Windows.h>

//////////////////////////////////////////////////////////
class TFrequencyTimer
{
private:

    // PerformanceCounter 를 PerformanceFrequency 값으로 나누면 초단위의 경과 시간을 구할 수 있다.
    LARGE_INTEGER m_liFrequency;        // clock 주파수 구하는 변수
    LARGE_INTEGER m_liPerfCounter;      // PC가 On 된 후 부터 현재 까지의 clock 수를 구하는 변수

    bool    m_bStarted;         //  Timer 동작 상황
    UINT    m_nLimitTime;       //  단위 : msec
    double  m_dwStartTime;      //  단위 : msec
    double  m_dwCrntTime;       //  단위 : msec
    double  m_dwStartTimeSec;   //  단위 : sec.

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

    double  StartTimer(UINT nTimeOutMSEC)   // timer를 시작한다. (현재 시간을 msec단위를 리턴한다)
    {
        QueryPerformanceCounter(&m_liPerfCounter);
        m_bStarted       = true;
        m_nLimitTime     = nTimeOutMSEC;
        m_dwStartTime    = (((double)(m_liPerfCounter.QuadPart * 1000.0) / (double)(m_liFrequency.QuadPart)));
        m_dwStartTimeSec = ((double)(m_liPerfCounter.QuadPart / (double)(m_liFrequency.QuadPart)));
        return m_dwStartTime;
    };

    double  StartTimerSEC(UINT nTimeOutSEC)   // timer를 시작한다. (현재 시간을 sec 단위로 리턴 한다.)
    {
        QueryPerformanceCounter(&m_liPerfCounter);
        m_bStarted       = true;
        m_nLimitTime     = nTimeOutSEC * 1000.0;
        m_dwStartTime    = (((double)(m_liPerfCounter.QuadPart * 1000.0) / (double)(m_liFrequency.QuadPart)));
        m_dwStartTimeSec = ((double)(m_liPerfCounter.QuadPart / (double)(m_liFrequency.QuadPart)));
        return m_dwStartTimeSec;
    };

    double  GetLapTimeMM()              // msec 단위로 경과 시간을 얻어온다.
    {
        QueryPerformanceCounter(&m_liPerfCounter);
        m_dwCrntTime = (((double)(m_liPerfCounter.QuadPart * 1000.0) / (double)(m_liFrequency.QuadPart))); //

        return (m_dwCrntTime - m_dwStartTime);
    };

    double  GetLapTimeMM(double dStartTime)              // msec 단위로 경과 시간을 얻어온다.
    {
        QueryPerformanceCounter(&m_liPerfCounter);
        m_dwCrntTime = (((double)(m_liPerfCounter.QuadPart * 1000.0) / (double)(m_liFrequency.QuadPart))); //

        return (m_dwCrntTime - dStartTime);
    };

    double  GetLapTimeSEC()             // sec 단위로 경과 시간을 얻어온다.
    {
        QueryPerformanceCounter(&m_liPerfCounter);
        m_dwCrntTime = (((double)(m_liPerfCounter.QuadPart) / (double)(m_liFrequency.QuadPart))); //

        return (m_dwCrntTime - m_dwStartTimeSec);
    };

    double  GetLapTimeSEC(double dStartTimeSec)             // sec 단위로 경과 시간을 얻어온다.
    {
        QueryPerformanceCounter(&m_liPerfCounter);
        m_dwCrntTime = (((double)(m_liPerfCounter.QuadPart) / (double)(m_liFrequency.QuadPart))); //

        return (m_dwCrntTime - dStartTimeSec);
    };


    bool    IsTimeOut()                 // timeout 되었는지 확인한다.
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

    bool    IsTimeOut(double dStartTime)                 // timeout 되었는지 확인한다.
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

