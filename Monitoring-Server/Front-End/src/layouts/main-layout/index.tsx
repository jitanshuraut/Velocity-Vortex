import { useState, PropsWithChildren } from 'react';
import Stack from '@mui/material/Stack';
import Sidebar from 'layouts/main-layout/sidebar';
import Topbar from 'layouts/main-layout/topbar';

const MainLayout = ({ children }: PropsWithChildren) => {
  const [mobileOpen, setMobileOpen] = useState(false);
  const [isClosing, setIsClosing] = useState(false);

  return (
    <Stack width={1} minHeight="100vh">
      <Sidebar mobileOpen={mobileOpen} setMobileOpen={setMobileOpen} setIsClosing={setIsClosing} />
      <Stack
        component="main"
        direction="column"
        px={3}
        width={{ xs: 1, lg: `calc(100% - 300px)` }}
        flexGrow={1}
      >
        <Topbar isClosing={isClosing} mobileOpen={mobileOpen} setMobileOpen={setMobileOpen} />
        {children}
      </Stack>
    </Stack>
  );
};

export default MainLayout;
