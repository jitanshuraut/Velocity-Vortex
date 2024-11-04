import { MenuItem } from 'routes/sitemap';
import Link from '@mui/material/Link';
import ListItemIcon from '@mui/material/ListItemIcon';
import ListItemButton from '@mui/material/ListItemButton';
import ListItemText from '@mui/material/ListItemText';
import IconifyIcon from 'components/base/IconifyIcon';
import { updateActiveState } from 'routes/sitemap';

const ListItem = ({ id, subheader, icon, path, active }: MenuItem) => {
  const handleClick = () => {
    updateActiveState(id);
  };

  return (
    <ListItemButton
      component={Link}
      href={path}
      onClick={handleClick}
      sx={{
        mb: 2.5,
        bgcolor: active ? 'primary.main' : null,
        '&:hover': {
          bgcolor: active ? 'primary.main' : null,
        },
      }}
    >
      <ListItemIcon>
        {icon && (
          <IconifyIcon
            icon={icon}
            fontSize="h4.fontSize"
            sx={{
              color: active ? 'info.light' : null,
            }}
          />
        )}
      </ListItemIcon>
      <ListItemText
        primary={subheader}
        sx={{
          '& .MuiListItemText-primary': {
            color: active ? 'info.light' : null,
          },
        }}
      />
    </ListItemButton>
  );
};

export default ListItem;
