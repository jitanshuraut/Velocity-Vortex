import Box from '@mui/material/Box';
import Stack from '@mui/material/Stack';
import Paper from '@mui/material/Paper';
import Typography from '@mui/material/Typography';
import SpentChart from './SpentChart';

const Spent = ({ balance }: { balance: number }) => {
  return (
    <Paper component={Stack} alignItems="center" justifyContent="space-between" sx={{ py: 2.5 }}>
      <Box>
        <Typography variant="body2" color="text.disabled" fontWeight={500}>
          Trades this month
        </Typography>
        <Typography mt={1} variant="h3">
          ${balance}
        </Typography>
      </Box>

      <SpentChart data={[160, 100, 210, 270, 180]} sx={{ width: 75, height: '68px !important' }} />
    </Paper>
  );
};

export default Spent;
