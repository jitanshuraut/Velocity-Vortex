import { Suspense, lazy } from 'react';
import { Outlet, createBrowserRouter } from 'react-router-dom';
import MainLayout from 'layouts/main-layout';
import Splash from 'components/loader/Splash';
import PageLoader from 'components/loader/PageLoader';
import Logs from 'pages/Logs/Logs';
import Portfolio from 'pages/portfolio/Portfolio';

const App = lazy(() => import('App'));
const Dashboard = lazy(() => import('pages/dashboard/Dashbaord'));

const router = createBrowserRouter(
  [
    {
      element: (
        <Suspense fallback={<Splash />}>
          <App />
        </Suspense>
      ),
      children: [
        {
          path: '/',
          element: (
            <MainLayout>
              <Suspense fallback={<PageLoader />}>
                <Outlet />
              </Suspense>
            </MainLayout>
          ),
          children: [
            {
              index: true,
              element: <Dashboard />,
            },
          ],
        },
      ],
    },

    {
      element: (
        <Suspense fallback={<Splash />}>
          <App />
        </Suspense>
      ),
      children: [
        {
          path: '/logs',
          element: (
            <MainLayout>
              <Suspense fallback={<PageLoader />}>
                <Outlet />
              </Suspense>
            </MainLayout>
          ),
          children: [
            {
              index: true,
              element: <Logs />,
            },
          ],
        },
      ],
    },

    {
      element: (
        <Suspense fallback={<Splash />}>
          <App />
        </Suspense>
      ),
      children: [
        {
          path: '/Portfolio',
          element: (
            <MainLayout>
              <Suspense fallback={<PageLoader />}>
                <Outlet />
              </Suspense>
            </MainLayout>
          ),
          children: [
            {
              index: true,
              element: <Portfolio />,
            },
          ],
        },
      ],
    },
  ],
  {
    basename: '/Velocity-Vortex',
  },
);

export default router;
