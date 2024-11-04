import Chip from '@mui/material/Chip';
import Paper from '@mui/material/Paper';
import Stack from '@mui/material/Stack';
import Typography from '@mui/material/Typography';
import IconifyIcon from 'components/base/IconifyIcon';
import EarningsChart from './EarningsChart';

const Earnings = () => {
  return (
    <Paper sx={{ px: 0, height: 355 }}>
      <Stack
        ml="auto"
        mr={3.5}
        alignItems="center"
        justifyContent="center"
        height={35}
        width={35}
        bgcolor="info.main"
        borderRadius={1.75}
      >
        <IconifyIcon icon="ic:round-bar-chart" color="primary.main" fontSize="h4.fontSize" />
      </Stack>

      <Typography variant="body2" textAlign="center" color="text.disabled" fontWeight={500}>
        This month earnings
      </Typography>
      <Typography mt={0.5} variant="h2" textAlign="center" color="text.primary">
        $682.5
      </Typography>

      <Stack mt={2} justifyContent="center">
        <Chip label="+2.45%" color="success" />
      </Stack>

      <EarningsChart data={[20, 30, 50, 45, 60, 20, 40, 50, 30, 60, 40, 50, 20, 45, 30]} sx={{ width: 1, height: '220px !important' }} />
    </Paper>
  );
};

export default Earnings;
