import Box from '@mui/material/Box';
import Paper from '@mui/material/Paper';
import Stack from '@mui/material/Stack';
import Typography from '@mui/material/Typography';
import IconifyIcon from 'components/base/IconifyIcon';
import SpentThisMonthChart from './SpentThisMonthChart';

const SpentThisMonth = () => {
  return (
    <Paper sx={{ height: 355 }}>
      <Stack justifyContent="space-between" alignItems="flex-start">
        <Box>
          <Typography variant="body2" color="text.disabled" fontWeight={500}>
            Spent this month
          </Typography>
          <Typography mt={0.35} variant="h2" color="text.primary">
            $682.5
          </Typography>
          <Stack mt={0.55} alignItems="center" spacing={0.5}>
            <IconifyIcon icon="ic:round-check-circle" color="success.main" fontSize="h6.fontSize" />
            <Typography variant="body1" color="success.main" fontWeight={700}>
              On track
            </Typography>
          </Stack>
        </Box>

        <Stack alignItems="center" spacing={0.25}>
          <IconifyIcon
            icon="ic:baseline-arrow-drop-up"
            color="success.main"
            fontSize="h6.fontSize"
          />
          <Typography variant="body2" color="success.main" fontWeight={700}>
            +2.45%
          </Typography>
        </Stack>
      </Stack>

      <SpentThisMonthChart
        data={[110, 80, 150, 100, 130, 160, 60]}
        sx={{ height: '230px !important' }}
      />
    </Paper>
  );
};

export default SpentThisMonth;
