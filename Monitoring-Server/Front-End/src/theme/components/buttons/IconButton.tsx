import { Theme } from '@mui/material';
import { Components } from '@mui/material/styles/components';

const IconButton: Components<Omit<Theme, 'components'>>['MuiIconButton'] = {
  styleOverrides: {
    root: ({ theme }) => ({
      color: theme.palette.text.primary,
      backgroundColor: theme.palette.info.lighter,
      marginLeft: 0,
    }),
    sizeLarge: ({ theme }) => ({
      padding: theme.spacing(1),
      fontSize: theme.typography.h3.fontSize,
    }),
    sizeMedium: ({ theme }) => ({
      padding: theme.spacing(0.75),
      fontSize: theme.typography.h4.fontSize,
    }),
    sizeSmall: ({ theme }) => ({
      padding: theme.spacing(0.5),
      fontSize: theme.typography.h6.fontSize,
    }),
  },
};

export default IconButton;
