import Link from '@mui/material/Link';
import Stack from '@mui/material/Stack';
import Toolbar from '@mui/material/Toolbar';
import TextField from '@mui/material/TextField';
import ButtonBase from '@mui/material/ButtonBase';
import IconButton from '@mui/material/IconButton';
import InputAdornment from '@mui/material/InputAdornment';
import IconifyIcon from 'components/base/IconifyIcon';
import Image from 'components/base/Image';
import LogoImg from 'assets/images/logo.png';

interface TopbarProps {
  isClosing: boolean;
  mobileOpen: boolean;
  setMobileOpen: React.Dispatch<React.SetStateAction<boolean>>;
}

const Topbar = ({ isClosing, mobileOpen, setMobileOpen }: TopbarProps) => {
  const handleDrawerToggle = () => {
    if (!isClosing) {
      setMobileOpen(!mobileOpen);
    }
  };

  return (
    <Stack
      py={3.5}
      alignItems="center"
      justifyContent="space-between"
      bgcolor="transparent"
      zIndex={1200}
    >
      <Stack spacing={{ xs: 2, sm: 3 }} alignItems="center">
        <ButtonBase
          component={Link}
          href="/"
          disableRipple
          sx={{ lineHeight: 0, display: { xs: 'none', sm: 'block', lg: 'none' } }}
        >
          <Image src={LogoImg} alt="logo" height={40} width={40} />
        </ButtonBase>

        <Toolbar sx={{ display: { xm: 'block', lg: 'none' } }}>
          <IconButton
            size="large"
            edge="start"
            color="inherit"
            aria-label="menu"
            onClick={handleDrawerToggle}
          >
            <IconifyIcon icon="ic:baseline-menu" />
          </IconButton>
        </Toolbar>

        <Toolbar sx={{ ml: -1.5, display: { xm: 'block', md: 'none' } }}>
          <IconButton size="large" edge="start" color="inherit" aria-label="search">
            <IconifyIcon icon="eva:search-fill" />
          </IconButton>
        </Toolbar>

        <TextField
          variant="filled"
          placeholder="Stock"
          sx={{ width: 340, display: { xs: 'none', md: 'flex' } }}
          InputProps={{
            startAdornment: <InputAdornment position="start"></InputAdornment>,
          }}
        />
      </Stack>
    </Stack>
  );
};

export default Topbar;
