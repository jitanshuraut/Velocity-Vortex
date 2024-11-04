import { Theme } from '@mui/material';
import { Components } from '@mui/material/styles/components';

const Drawer: Components<Omit<Theme, 'components'>>['MuiDrawer'] = {
  styleOverrides: {
    root: {
      '&:hover, &:focus': {
        '*::-webkit-scrollbar, *::-webkit-scrollbar-thumb': {
          visibility: 'visible',
        },
      },
    },
    paper: ({ theme }) => ({
      padding: 0,
      width: '290px',
      height: '100vh',
      border: 0,
      borderRadius: 0,
      backgroundColor: theme.palette.info.light,
      boxShadow: 'none',
      boxSizing: 'border-box',
    }),
  },
};

export default Drawer;
