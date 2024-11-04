import { Theme } from '@mui/material';
import { Components } from '@mui/material/styles/components';

const Button: Components<Omit<Theme, 'components'>>['MuiButton'] = {
  defaultProps: {
    disableElevation: true,
  },
  styleOverrides: {
    root: ({ theme }) => ({
      color: theme.palette.info.light,
      borderRadius: theme.shape.borderRadius * 10,
      textTransform: 'initial',
    }),
    text: ({ theme }) => ({
      color: theme.palette.text.disabled,
      backgroundColor: 'transparent !important',
    }),
    containedPrimary: ({ theme }) => ({
      color: theme.palette.info.light,
      backgroundColor: theme.palette.primary.main,
      '&:hover': { backgroundColor: theme.palette.primary.main },
    }),
    containedSecondary: ({ theme }) => ({
      color: theme.palette.text.primary,
      backgroundColor: theme.palette.info.dark,
      '&:hover': { backgroundColor: theme.palette.info.dark },
    }),
    sizeLarge: ({ theme }) => ({
      padding: theme.spacing(1.125, 3),
      fontSize: theme.typography.h6.fontSize,
      fontWeight: 600,
    }),
    sizeMedium: ({ theme }) => ({
      padding: theme.spacing(1, 2),
      fontSize: theme.typography.body1.fontSize,
      fontWeight: 600,
    }),
    sizeSmall: ({ theme }) => ({
      padding: theme.spacing(0.875, 1.5),
      fontSize: theme.typography.caption.fontSize,
      fontWeight: 700,
    }),
    startIcon: {
      marginRight: 10,
    },
    endIcon: {
      marginLeft: 6,
    },
  },
};

export default Button;