import Box from '@mui/material/Box';
import Stack from '@mui/material/Stack';
import Paper from '@mui/material/Paper';
import Typography from '@mui/material/Typography';
import ActivityChart from './ActivityChart';

const Activity = () => {
  return (
    <Paper
      component={Stack}
      alignItems="center"
      justifyContent="space-between"
      sx={(theme) => ({
        px: 3,
        py: 2.5,
        background: `linear-gradient(135deg, ${theme.palette.gradients.primary.state} 0%, ${theme.palette.gradients.primary.main} 100%)`,
      })}
    >
      <Box>
        <Typography variant="body2" color="info.dark" fontWeight={500}>
          Activity
        </Typography>
        <Typography mt={1} variant="h3" color="info.light">
          $540.50
        </Typography>
      </Box>

      <ActivityChart data={[15, 50, 30, 45, 50]} sx={{ width: 75, height: '68px !important' }} />
    </Paper>
  );
};

export default Activity;
