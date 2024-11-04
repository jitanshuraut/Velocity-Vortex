import { Theme } from '@mui/material';
import { Components } from '@mui/material/styles/components';

const Badge: Components<Omit<Theme, 'components'>>['MuiBadge'] = {
  styleOverrides: {
    root: {},
    badge: {
      top: 4,
      right: 4,
    },
  },
};

export default Badge;