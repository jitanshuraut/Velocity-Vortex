import IconifyIcon from 'components/base/IconifyIcon';
import Paper from '@mui/material/Paper';
import Stack from '@mui/material/Stack';
import Typography from '@mui/material/Typography';
import FormControl from '@mui/material/FormControl';
import BalanceChart from './BalanceChart';

const Balance = ({ title }: { title: string }) => {
  let base: number = +new Date(1968, 9, 3);
  const oneDay: number = 24 * 3600 * 1000;
  const date: string[] = [];
  const chartData: number[] = [Math.random() * 300];
  for (let i = 1; i < 20; i++) {
    const now: Date = new Date((base += oneDay));
    date.push([now.getFullYear(), now.getMonth() + 1, now.getDate()].join('/'));
    chartData.push(Math.round((Math.random() - 0.5) * 20 + chartData[i - 1]));
  }

  return (
    <Paper sx={{ height: { xs: 500, sm: 550 } }}>
      <Stack alignItems={{ xs: 'flex-start', sm: 'center' }} justifyContent="space-between">
        <Stack
          alignItems="center"
          spacing={{ xs: 0.75, sm: 1.5 }}
          direction={{ xs: 'column', sm: 'row' }}
        >
          <Typography variant="h4" color="text.primary">
            {title}
          </Typography>
          <Stack mt={{ xs: 0, sm: 0.55 }} alignItems="center" spacing={0.5}>
            <IconifyIcon icon="ic:round-check-circle" color="success.main" fontSize="h6.fontSize" />
            <Typography variant="body1" color="success.main" fontWeight={700}>
              On track
            </Typography>
          </Stack>
        </Stack>

        <FormControl
          variant="filled"
          sx={{
            width: 105,
            '& .MuiInputBase-root': {
              '&:focus-within': {
                borderColor: 'transparent !important',
                boxShadow: 'none',
              },
            },
          }}
        ></FormControl>
      </Stack>

      <BalanceChart
        date={date}
        chartData={chartData}
        sx={{ height: '420px !important' }}
      />
    </Paper>
  );
};

export default Balance;
