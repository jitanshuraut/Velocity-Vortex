import { createTheme } from '@mui/material/styles';
import type {} from '@mui/x-data-grid/themeAugmentation';
import palette from './palette';
import typography from './typography';
import customShadows from './shadows';
import Stack from './components/layout/Stack';
import Drawer from './components/navigation/Drawer';
import Paper from './components/surfaces/Paper';
import CssBaseline from './components/utils/CssBaseline';
import List from './components/list/List';
import Button from './components/buttons/Button';
import ButtonBase from './components/buttons/ButtonBase';
import ListItemButton from './components/list/ListItemButton';
import ListItemIcon from './components/list/ListItemIcon';
import ListItemText from './components/list/ListItemText';
import Collapse from './components/list/Collapse';
import MenuItem from './components/list/MenuItem';
import Link from './components/navigation/Link';
import IconButton from './components/buttons/IconButton';
import Toolbar from './components/buttons/Toolbar';
import FilledInput from './components/inputs/FilledInput';
import FormControlLabel from './components/inputs/FormControlLabel';
import InputBase from './components/inputs/InputBase';
import OutlinedInput from './components/inputs/OutlinedInput';
import InputAdornment from './components/inputs/InputAdornment';
import Checkbox from './components/inputs/Checkbox';
import Select from './components/inputs/Select';
import Badge from './components/data-display/Badge';
import Chip from './components/data-display/Chip';
import DataGrid from './components/data-grid/DataGrid';
import PaginationItem from './components/pagination/PaginationItem';

export const theme = createTheme({
  typography,
  palette,
  customShadows,
  components: {
    MuiStack: Stack,
    MuiDrawer: Drawer,
    MuiLink: Link,
    MuiPaper: Paper,
    MuiButton: Button,
    MuiButtonBase: ButtonBase,
    MuiIconButton: IconButton,
    MuiToolbar: Toolbar,
    MuiBadge: Badge,
    MuiChip: Chip,
    MuiList: List,
    MuiCollapse: Collapse,
    MuiMenuItem: MenuItem,
    MuiListItemButton: ListItemButton,
    MuiListItemIcon: ListItemIcon,
    MuiListItemText: ListItemText,
    MuiDataGrid: DataGrid,
    MuiSelect: Select,
    MuiCheckbox: Checkbox,
    MuiInputBase: InputBase,
    MuiFilledInput: FilledInput,
    MuiOutlinedInput: OutlinedInput,
    MuiInputAdornment: InputAdornment,
    MuiFormControlLabel: FormControlLabel,
    MuiPaginationItem: PaginationItem,
    MuiCssBaseline: CssBaseline,
  },
});
