import { Theme } from '@mui/material';
import { Components } from '@mui/material/styles/components';

const Chip: Components<Omit<Theme, 'components'>>['MuiChip'] = {
  styleOverrides: {
    root: {
      margin: 0,
      fontWeight: 700,
    },
    sizeSmall: ({ theme }) => ({
      height: 24,
      padding: theme.spacing(0, 1),
      fontSize: theme.typography.caption.fontSize,
    }),
    sizeMedium: ({ theme }) => ({
      height: 28,
      padding: theme.spacing(0, 1.25),
      fontSize: theme.typography.body2.fontSize,
    }),
    colorPrimary: ({ theme }) => ({
      color: theme.palette.text.primary,
      backgroundColor: theme.palette.info.light,
    }),
    colorSuccess: ({ theme }) => ({
      color: theme.palette.success.main,
      backgroundColor: theme.palette.transparent.success.main,
    }),
    colorWarning: ({ theme }) => ({
      color: theme.palette.warning.main,
      backgroundColor: theme.palette.transparent.warning.main,
    }),
    colorError: ({ theme }) => ({
      color: theme.palette.error.main,
      backgroundColor: theme.palette.transparent.error.main,
    }),
    iconSmall: {
      width: 12,
      margin: '0 !important',
    },
    iconMedium: {
      width: 16,
      margin: '0 !important',
    },
    labelSmall: {
      padding: 0,
      textTransform: 'capitalize',
    },
    labelMedium: {
      padding: 0,
      textTransform: 'capitalize',
    },
  },
};

export default Chip;