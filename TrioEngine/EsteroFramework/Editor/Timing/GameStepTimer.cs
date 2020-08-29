
using System;
using System.Diagnostics;
using System.Windows.Data;

namespace EsteroFramework.Editor.Timing
{
    public class GameStepTimer
    {
        private bool m_isFixedTimeStep;
        private TimeSpan m_qpcLastTime = TimeSpan.Zero;

        private TimeSpan m_maxStepDelta = TimeSpan.Zero;
        private TimeSpan m_leftOverTime = TimeSpan.Zero;

        private TimeSpan m_qpcSecondCounter = TimeSpan.Zero;
        private int m_framesThisSecond;
        private int m_framesPerSecond;

        private int m_frameCount = 0;

        public TimeSpan ElapsedTime
        {
            get
            {
                return m_elapsedTime;
            }
        }
        private TimeSpan m_elapsedTime = TimeSpan.Zero;

        public TimeSpan TotalTime
        {
            get
            {
                return m_totalTime;
            }
        }
        private TimeSpan m_totalTime = TimeSpan.Zero;

        public TimeSpan TargetElapsedTime
        {
            get
            {
                return m_targetElapsedTime;
            }
            set
            {
                m_targetElapsedTime = value;
            }
        }
        private TimeSpan m_targetElapsedTime = TimeSpan.Zero;

        private TimeSpan StepSizeTolerance
        {
            get
            {
                return m_stepSizeTolerance;
            }
            set
            {
                m_stepSizeTolerance = value;
            }
        }
        private TimeSpan m_stepSizeTolerance = TimeSpan.Zero;

        public bool IsFixedTimeStep
        {
            get
            {
                return m_isFixedTimeStep;
            }
            set
            {
                m_isFixedTimeStep = value;
            }
        }

        private Stopwatch _stopwatch;

        public delegate void TickEvent();
        public event TickEvent OnTimeChanged;

        public GameStepTimer()
        {
            _stopwatch = new Stopwatch();

            m_isFixedTimeStep = false;
            m_maxStepDelta = TimeSpan.FromTicks(Stopwatch.Frequency / 10);
            m_targetElapsedTime = TimeSpan.FromTicks(TimeSpan.TicksPerSecond / 60);
            m_stepSizeTolerance = TimeSpan.FromTicks(TimeSpan.TicksPerSecond / 4000);
        }

        public void Start()
        {
            _stopwatch.Start();
        }

        public void Stop()
        {
            _stopwatch.Stop();

            m_leftOverTime = TimeSpan.Zero;
            m_framesPerSecond = 0;
            m_framesThisSecond = 0;
            m_qpcSecondCounter = TimeSpan.Zero;
            m_qpcLastTime = TimeSpan.Zero;
        }

        public void Tick()
        {
            var currentTime = _stopwatch.Elapsed;
            var timeDelta = currentTime - m_qpcLastTime;

            m_qpcLastTime = currentTime;
            m_qpcSecondCounter += timeDelta;

            if (timeDelta > m_maxStepDelta)
            {
                timeDelta = m_maxStepDelta;
            }

            int lastFrameCount = m_frameCount;
            if (m_isFixedTimeStep)
            {
                if (Math.Abs((timeDelta - m_targetElapsedTime).Ticks) < m_stepSizeTolerance.Ticks)
                {
                    timeDelta = m_targetElapsedTime;
                }

                m_leftOverTime += timeDelta;

                while (m_leftOverTime >= m_targetElapsedTime)
                {
                    m_elapsedTime = m_targetElapsedTime;
                    m_totalTime += m_targetElapsedTime;
                    m_leftOverTime -= m_targetElapsedTime;
                    m_frameCount++;

                    OnTimeChanged();
                }
            }
            else
            {
                m_elapsedTime = timeDelta;
                m_totalTime += timeDelta;
                m_leftOverTime = TimeSpan.Zero;
                m_frameCount++;

                OnTimeChanged();
            }

            if (m_frameCount != lastFrameCount)
            {
                m_framesThisSecond++;
            }

            if (m_qpcSecondCounter.Ticks >= Stopwatch.Frequency)
            {
                m_framesPerSecond = m_framesThisSecond;
                m_framesThisSecond = 0;

                var secondCounterTicks = m_qpcSecondCounter.Ticks;
                secondCounterTicks %= Stopwatch.Frequency;
                m_qpcSecondCounter = TimeSpan.FromTicks(secondCounterTicks);

                Console.WriteLine(string.Format("FPS = {0}", m_framesPerSecond));
            }
        }
    }
}
