import { Theme } from '@mui/material';
import { Components } from '@mui/material/styles/components';

const Select: Components<Omit<Theme, 'components'>>['MuiSelect'] = {
  styleOverrides: {
    root: {
      padding: 0,
      '&.MuiInputBase-root': {
        backgroundColor: 'transparent !important',
        border: 'none',
      },
    },
    select: ({ theme }) => ({
      padding: theme.spacing(1),
      paddingLeft: theme.spacing(1.25),
      paddingRight: '0 !important',
      backgroundColor: 'transparent !important',
      fontSize: theme.typography.body2.fontSize,
      color: theme.palette.text.disabled,
      fontWeight: 600,
    }),
    icon: ({ theme }) => ({
      color: theme.palette.text.disabled,
      fontSize: theme.typography.h4.fontSize,
    }),
  },
};

export default Select;
