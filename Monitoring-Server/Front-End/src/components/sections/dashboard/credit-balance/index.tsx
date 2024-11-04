import Box from '@mui/material/Box';
import Stack from '@mui/material/Stack';
import Paper from '@mui/material/Paper';
import Typography from '@mui/material/Typography';
import IconButton from '@mui/material/IconButton';
import IconifyIcon from 'components/base/IconifyIcon';
import BalanceBG from 'assets/images/balance-bg.png';
import CreditBalanceChart from './CreditBalanceChart';
import customShadows from 'theme/shadows';

interface CreditBalanceProps {
  id: number | string;
  type: string;
  time: string;
  amount: string;
  icon: string;
  color: string;
}

const creditBalanceData: CreditBalanceProps[] = [
  {
    id: 1,
    type: 'Bill & Taxes',
    time: 'Today, 16:36',
    amount: '-$154.50',
    icon: 'ic:baseline-domain',
    color: 'secondary.light',
  },
  {
    id: 2,
    type: 'Car Energy',
    time: '23 Jun, 13:06',
    amount: '-$40.50',
    icon: 'ic:baseline-electric-car',
    color: 'success.main',
  },
  {
    id: 3,
    type: 'Design Course',
    time: '21 Jun, 19:04',
    amount: '-$70.00',
    icon: 'ic:outline-school',
    color: 'warning.main',
  },
];

const CreditBalance = () => {
  return (
    <Paper sx={{ height: 396 }}>
      <Box
        p={2}
        height={100}
        borderRadius={4}
        sx={{
          backgroundImage: `url('${BalanceBG}')`,
          backgroundRepeat: 'no-repeat',
          backgroundSize: 'cover',
        }}
        boxShadow={customShadows[0]}
      >
        <Stack justifyContent="space-between">
          <Typography variant="body2" color="info.dark" fontWeight={500}>
            Credit Balance
          </Typography>

          <IconButton size="medium" sx={{ mt: -1, mr: -0.5 }}>
            <IconifyIcon icon="solar:menu-dots-bold" color="info.light" />
          </IconButton>
        </Stack>

        <Stack justifyContent="space-between">
          <Typography variant="h2" color="info.light">
            $25,215
          </Typography>

          <CreditBalanceChart
            data={[25, 12, 50, 40, 60]}
            sx={{ width: 80, height: '50px !important' }}
          />
        </Stack>
      </Box>

      <Typography mt={2} variant="body2" color="text.disabled" fontWeight={500}>
        Recent
      </Typography>

      {creditBalanceData.map((item) => (
        <Stack key={item.id} mt={1.25} mb={2.75} alignItems="center" justifyContent="space-between">
          <Stack spacing={2} alignItems="center" justifyContent="flex-start">
            <Stack
              alignItems="center"
              justifyContent="center"
              height={48}
              width={48}
              bgcolor="info.dark"
              borderRadius="50%"
            >
              <IconifyIcon icon={item.icon} color={item.color} fontSize="h4.fontSize" />
            </Stack>
            <Stack direction="column">
              <Typography variant="body1" fontWeight={700}>
                {item.type}
              </Typography>
              <Typography variant="caption" color="text.disabled" fontWeight={500}>
                {item.time}
              </Typography>
            </Stack>
          </Stack>

          <Typography variant="body1" fontWeight={700}>
            {item.amount}
          </Typography>
        </Stack>
      ))}
    </Paper>
  );
};

export default CreditBalance;
